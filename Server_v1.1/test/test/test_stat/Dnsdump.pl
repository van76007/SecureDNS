#!/usr/bin/perl -T

# $Id: dnsdump,v 1.11 2010/03/20 00:04:39 wessels Exp $

# URL: http://dns.measurement-factory.com/tools/dnsdump/

# Copyright (c) 2006, The Measurement Factory, Inc.  All rights
# reserved.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:

# Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
# Neither the name of The Measurement Factory nor the names of its
# contributors may be used to endorse or promote products derived
# from this software without specific prior written permission.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# AUTHORS/CONTRIBUTORS:
#
# Duane Wessels  wessels measurement-factory.com
# John Kristoff	 jtk depaul.edu
# Wayne Beaver   wbeaver afilias.info
#

use warnings;
use strict;

# Have to clear some of %ENV because Net::Packet::Env has a backtick
# that taint mode doesn't like
#
BEGIN { $ENV{PATH}=''; }
BEGIN { $ENV{ENV}=''; }

use Switch 'Perl6';
use Data::Dumper;
use Getopt::Std;
use Net::Pcap;
use IO::Socket::INET;
use Net::Packet::ETH;
use Net::Packet::VLAN;
use Net::Packet::IPv4;
use Net::Packet::IPv6;
use Net::Packet::TCP;
use Net::Packet::UDP;
use Net::Packet::Consts qw(:eth :sll);
use Net::DNS::Packet;
use Net::DNS::Header;
use Net::DNS::RR;

my %opts;
getopts('aqi:r:f', \%opts);
usage() unless $opts{i} || $opts{r};
usage() if $opts{i} && $opts{r};

my $format = join(' ', @ARGV) || "%time %src %dst %qtype? %qname %ans\n";

my $err;
my $pcap;

if ($opts{i}) {
	$pcap = Net::Pcap::open_live($opts{i}, 1500, 0, 1000, \$err);
} else {
	$pcap = Net::Pcap::open_offline($opts{r}, \$err);
}

unless (defined ($pcap)) {
	print STDERR "$err\n";
	exit(1);
}

my $grok_pkt_dlt;
given (Net::Pcap::pcap_datalink($pcap)) {
	when (Net::Pcap::DLT_EN10MB)	{ $grok_pkt_dlt = \&grok_pkt_dlt_eth; }
	when (Net::Pcap::DLT_RAW)	{ $grok_pkt_dlt = \&grok_pkt_dlt_raw; }
	when (Net::Pcap::DLT_LOOP)	{ $grok_pkt_dlt = \&grok_pkt_dlt_loop; }
	when (Net::Pcap::DLT_LINUX_SLL) { $grok_pkt_dlt = \&grok_pkt_dlt_sll; }
	default				{ die "DLT ". Net::Pcap::pcap_datalink_val_to_name(Net::Pcap::pcap_datalink($pcap)). " unsupported\n"; }
}

my $filter;
my $filter_str = 'udp port 53';
die if (Net::Pcap::compile($pcap, \$filter, $filter_str, 0, 0));
Net::Pcap::setfilter($pcap, $filter);

Net::Pcap::loop($pcap, -1, \&process_pkt, undef);
exit(0);

sub process_pkt {
	my($user_data, $pcap_hdr, $pkt) = @_;

	my $rec = &$grok_pkt_dlt($pkt);
	return unless $rec;

	# query only
	return if ($opts{a} && !$opts{q} && $rec->{qr} == 0);
	# answer only

	return if ($opts{q} && !$opts{a} && $rec->{qr} == 1);

	$rec->{time} = sprintf "%d.%06d", $pcap_hdr->{tv_sec}, $pcap_hdr->{tv_usec};
	#print Dumper($rec);
	foreach my $t (split(/%/, $format)) {
		next unless $t;
		die "t=$t" unless ($t =~ /^([-\d]*)([\(\[\{\<]*)([a-zA-Z_]+)([\]\)\>\}]*)(.*)/);
		my $width = $1;
		my $od = $2;
		my $key = $3;
		my $cd = $4;
		my $literal = $5;
		my $isarray = 0;
		$isarray = 1 if (ref($rec->{$key}) eq 'ARRAY');

		if ($isarray) {
			push(@{$rec->{$key}}, '') unless $rec->{$key};
			printf "%${width}s", join(' ', map {$od . $_ . $cd} @{$rec->{$key}});
		} elsif (defined($rec->{$key})) {
			printf "%${width}s", $rec->{$key};
		} else {
			printf "%${width}s", "- ";
		}
		print $literal;
	}
	print "\n";
}

sub grok_pkt_dlt_sll {
	my $pkt = shift;
	my $eth = Net::Packet::SLL->new(raw => $pkt);
	return unless $eth->payload;
	my $ip_v = 0;
	my $payload = undef;
	my $ip = undef;
	if ($eth->protocol == NP_SLL_PROTOCOL_IPv4) {
		$ip_v = 4;
		$payload = $eth->payload;
	} elsif ($eth->protocol == NP_SLL_PROTOCOL_IPv6) {
		$ip_v = 6;
		$payload = $eth->payload;
	} elsif ($eth->protocol == NP_SLL_PROTOCOL_VLAN) {
		my $vlan = undef;
		# might call die, so use eval
		eval { $vlan = Net::Packet::VLAN->new(raw => $eth->payload); };
		return unless $vlan;
		if ($vlan->type == 0x800) {
			$ip_v = 4;
			$payload = substr($eth->payload, 4);
		} elsif ($vlan->type == 0x86DD) {
			$ip_v = 6;
			$payload = substr($eth->payload, 4);
		} else {
			#warn "unknown vlan type ", $vlan->type, "\n";
			#warn $vlan->print."\n";
			#warn 'PAYLOAD: '.unpack('H*', $vlan->payload)."\n" if $vlan->payload;
		}
	} else {
		# Note well: Net/Packet/SLL also supports 
		# type NP_SLL_PROTOCOL_ARP but just elsing all which is
		# not v4|v6|VLAN, for now
		1 || warn "unknown ether type ", $eth->protocol, "\n";
	}
	return grok_pkt_ip($ip_v, $payload);
}

sub grok_pkt_dlt_eth {
	my $pkt = shift;
	my $eth = Net::Packet::ETH->new(raw => $pkt);
	return unless $eth->payload;
	my $ip_v = 0;
	my $payload = undef;
	my $ip = undef;
	if ($eth->type == NP_ETH_TYPE_IPv4) {
		$ip_v = 4;
		$payload = $eth->payload;
	} elsif ($eth->type == NP_ETH_TYPE_IPv6) {
		$ip_v = 6;
		$payload = $eth->payload;
	} elsif ($eth->type ==  NP_ETH_TYPE_VLAN) {
		my $vlan = undef;
		# might call die, so use eval
		eval { $vlan = Net::Packet::VLAN->new(raw => $eth->payload); };
		return unless $vlan;
		if ($vlan->type == 0x800) {
			$ip_v = 4;
			$payload = substr($eth->payload, 4);
		} elsif ($vlan->type == 0x86DD) {
			$ip_v = 6;
			$payload = substr($eth->payload, 4);
		} else {
			#warn "unknown vlan type ", $vlan->type, "\n";
			#warn $vlan->print."\n";
			#warn 'PAYLOAD: '.unpack('H*', $vlan->payload)."\n" if $vlan->payload;
		}
	} else {
		1 || warn "unknown ether type ", $eth->type, "\n";
	}
	return grok_pkt_ip($ip_v, $payload);
}

sub grok_pkt_dlt_raw {
	my $pkt = shift;
	my $ip_v = unpack('C', $pkt) >> 4;
	return grok_pkt_ip($ip_v, $pkt);
}

sub grok_pkt_dlt_loop {
	my $pkt = shift;
	my $family = unpack('N', $pkt);
	return grok_pkt_dlt_raw(substr($pkt,4));
}

sub grok_pkt_ip {
	my $ip_v = shift;
	my $pkt = shift;
	return unless $pkt;

	my $ip = undef;
	if (4 == $ip_v) {
		$ip = Net::Packet::IPv4->new(raw => $pkt);
	} elsif (6 == $ip_v) {
		$ip = Net::Packet::IPv6->new(raw => $pkt);
	}
	unless ($ip) {
		if ($opts{f}) {
			# treat parse error as fatal
			print hdump($pkt);
			die "Net::Packet::IPvX failed\n";
		}
		return;
	}

	my $rec;
	$rec->{src} = $ip->src;
	$rec->{dst} = $ip->dst;
	return unless ($ip->protocol == 17);

	my $udp = Net::Packet::UDP->new(raw => $ip->payload);
	return unless $udp;
	$rec->{sport} = $udp->src;
	$rec->{dport} = $udp->dst;
	return unless $udp->payload;

	my $dns = Net::DNS::Packet->new(\$udp->payload);
	unless ($dns) {
		warn "Net::DNS::Packet->new: $!";
		return;
	}

	my $hdr = $dns->header;
	$rec->{id} = $hdr->id;
	$rec->{qr} = $hdr->qr;
	$rec->{opcode} = $hdr->opcode;
	$rec->{rcode} = $hdr->rcode;
	$rec->{aa} = $hdr->aa;
	$rec->{tc} = $hdr->tc;
	$rec->{rd} = $hdr->rd;
	$rec->{ra} = $hdr->ra;
	$rec->{ad} = $hdr->ad;
	$rec->{qdcount} = $hdr->qdcount;
	$rec->{ancount} = $hdr->ancount;
	$rec->{nscount} = $hdr->nscount;
	$rec->{adcount} = $hdr->adcount;

	if ($dns->question) {
		foreach my $rr ($dns->question) {
			push(@{$rec->{question}}, rr_munge($rr->string, $rr->qtype));
			$rec->{qname} = $rr->qname unless defined $rec->{qname};
			$rec->{qtype} = $rr->qtype unless defined $rec->{qtype};
		}
	}
	if ($dns->answer) {
		foreach my $rr ($dns->answer) {
			push(@{$rec->{answer}}, rr_munge($rr->string, $rr->type));
			push(@{$rec->{ans}}, rr_munge($rr->rdatastr, $rr->type));
		}
	}
	if ($dns->authority) {
		foreach my $rr ($dns->authority) {
			push(@{$rec->{authority}}, rr_munge($rr->string, $rr->type));
			push(@{$rec->{auth}}, rr_munge($rr->rdatastr, $rr->type));
		}
	}
	if ($dns->additional) {
		foreach my $rr ($dns->additional) {
			push(@{$rec->{additional}}, rr_munge($rr->string, $rr->type));
			push(@{$rec->{addl}}, rr_munge($rr->rdatastr, $rr->type));
		}
	}
	$rec->{qname} = '.' if '' eq $rec->{qname};
	#catch { print "Caught Exception:\n", Dumper($dns); }
	return $rec;
}

# consolidate whitespace, remove comments from SOA and OPT RRs
#
sub rr_munge {
	my $x = shift;
	my $t = shift;
	$x =~ s/;.*$//mg if ($t eq 'SOA');
	$x =~ s/;.*$// if ($t eq 'OPT');
	$x =~ s/\s+/ /g;
	#$x =~ s/ /\//g;
	return $x;
}

sub usage {
	print STDERR "$0 [ -i ifname | -r savefile ] [ -q ] [ -a ] \"%field %field ...\"\n";
	print STDERR "\t-q\tqueries only\n";
	print STDERR "\t-a\tanswers only\n";
	print STDERR "\n";
	print STDERR "Fields are:\n";
	print STDERR "\ttime\t\tTime that the packet was received\n";
	print STDERR "\tsrc\t\tSource IP address\n";
	print STDERR "\tdst\t\tDestination IP address\n";
	print STDERR "\tsport\t\tSource port\n";
	print STDERR "\tdport\t\tDestination port\n";
	print STDERR "\tid\t\tQuery ID\n";
	print STDERR "\tqr\t\tQuery or Response (Query = 0, Response = 1)\n";
	print STDERR "\topcode\t\tOpcode (QUERY, NOTIFY, UPDATE, etc)\n";
	print STDERR "\trcode\t\tResponse code (NOERROR, NXDOMAIN, REFUSED, etc)\n";
	print STDERR "\taa\t\tAuthoritative Answer bit\n";
	print STDERR "\ttc\t\tTruncation bit \n";
	print STDERR "\trd\t\tRecursion Desired bit\n";
	print STDERR "\tra\t\tRecrusion Available bit\n";
	print STDERR "\tad\t\tAuthenticated Data bit\n";
	print STDERR "\tqcount\t\tCount of records in the Question section\n";
	print STDERR "\tancount\t\tCount of records in the Answer section\n";
	print STDERR "\tnscount\t\tCount of records in the Authority section\n";
	print STDERR "\tadcount\t\tCount of records in the Additional section\n";
	print STDERR "\tqname\t\tQuery Name (first Question only)\n";
	print STDERR "\tqtype\t\tQuery Type (first Question only)\n";
	print STDERR "\tans\t\tList of Answers (short form)\n";
	print STDERR "\tauth\t\tList of Authority records (short form)\n";
	print STDERR "\taddl\t\tList of Additional records (short form)\n";
	print STDERR "\tquestion\tList of Questions (long form)\n";
	print STDERR "\tanswer\t\tList of Answers (long form)\n";
	print STDERR "\tauthority\tList of Authority records (long form)\n";
	print STDERR "\tadditional\tList of Additional records (long form)\n";
	exit 1;
}

# http://www.perlmonks.org/?node_id=111481
# 
sub hdump {
    my $offset = 0;
    my(@array,$format);
    foreach my $data (unpack("a16"x(length($_[0])/16)."a*",$_[0])) {
        my($len)=length($data);
        if ($len == 16) {
            @array = unpack('N4', $data);
            $format="0x%08x (%05d)   %08x %08x %08x %08x   %s\n";
        } else {
            @array = unpack('C*', $data);
            $_ = sprintf "%2.2x", $_ for @array;
            push(@array, '  ') while $len++ < 16;
            $format="0x%08x (%05d)" .
               "   %s%s%s%s %s%s%s%s %s%s%s%s %s%s%s%s   %s\n";
        } 
        $data =~ tr/\0-\37\177-\377/./;
        printf $format,$offset,$offset,@array,$data;
        $offset += 16;
    }
}