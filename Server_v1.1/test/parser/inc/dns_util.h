/**
 * @file   dns_util.h
 * @author dvv <dvv@avauntguard.com>
 * @brief  Model structure of DNS request/response & provide parsing DNS packets functionalities
 */
 /*
 @ToDo:
 If you have an individual C function that you want to call, and for some reason you don't have or don't want to #include a C header file in which that function is declared, you can declare the individual C function in your C++ code using the extern "C" syntax. Naturally you need to use the full function prototype:
 extern "C" void f(int i, char c, float x);
A block of several C functions can be grouped via braces:

 extern "C" {
   void   f(int i, char c, float x);
   int    g(char* s, char const* s2);
   double sqrtOfSumOfSquares(double a, double b);
 }

After this you simply call the function just as if it were a C++ function:

 int main()
 {
   f(7, 'x', 3.14);   // Note: nothing unusual in the call
   ...
 }
 */
#ifndef __DNS_UTIL_H__
#define __DNS_UTIL_H__

#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <map>
#include <sys/types.h> 
#include <sys/socket.h>

#include <boost/tokenizer.hpp> // To split string for IP
#include <boost/algorithm/string.hpp> // To split querry into sub-domain

using namespace std;

enum DNS_Type
{
    TYPE_A = 1,
    TYPE_NS = 2,
    TYPE_MD = 3,
    TYPE_MF = 4,
    TYPE_CNAME = 5,
    TYPE_SOA = 6,
    TYPE_MB = 7,
    TYPE_MG = 8,
    TYPE_MR = 9,
    TYPE_NULL_ = 10,
    TYPE_WKS = 11,
    TYPE_PTR = 12,
    TYPE_HINFO = 13,
    TYPE_MINFO = 14,
    TYPE_MX = 15,
    TYPE_TXT = 16,
    TYPE_AAAA = 28
};

enum DNS_Class
{
    CLASS_IN = 1,
    CLASS_CS = 2,
    CLASS_CH = 3,
    CLASS_HS = 4,
    CLASS_ANY = 255
};

struct DNS_Question
{
    char name[256]; // This is the "human-readable" form of domain, not the one encoded inside packet
    enum DNS_Type dnsType;
    enum DNS_Class dnsClass;
};

struct DNS_RR
{
    char name[256];
    enum DNS_Type dnsType;
    enum DNS_Class dnsClass;
    unsigned int ttl;  
    unsigned int length;
    void* rdata;
};

struct DNS_RDATA_UNKNOWN
{
    char data[512];
};

struct DNS_RDATA_A
{
    struct in_addr address;
};

struct DNS_RDATA_AAAA
{
    struct in6_addr address;
};

struct DNS_RDATA_CNAME
{
    char name[256];
};

struct DNS_RDATA_NS
{
    char name[256];
};

struct DNS_RDATA_PTR
{
    char name[256];
};

struct DNS_RDATA_SOA
{
    char mname[256];
    char rname[256];
    unsigned long serial;
    unsigned long refresh;
    unsigned long retry;
    unsigned long expire;
    unsigned long minimum;
};

struct DNS_Packet
{
    // Should be grouped into struct DNS_Header? unsigned short: 2 bytes
    unsigned short id;
    // Each bit in flags can define information such as: query or response. 
    // Struct of flags for resp/req are different
    unsigned short flags;
    unsigned short qdcount;
    unsigned short ancount;
    unsigned short nscount;
    unsigned short arcount;
    // End struct DNS_Header
	
    struct DNS_Question** question;
    struct DNS_RR** answer;
    struct DNS_RR** authority;
    struct DNS_RR** additional;
};
// A reader object to parse DNS packet
union pointer
{
    // Length of data to be read after the pointer
    unsigned char* uchar; // data reader points to
    unsigned short* ushort;
    unsigned int* ulong; // change name later

    struct in_addr* in_addr;
    struct in6_addr* in6_addr;
    size_t* boundary; // Reader pointer should not over boundary
};

const char* DNS_Type_str(enum DNS_Type);
const char* DNS_Class_str(enum DNS_Class);

struct DNS_Packet* DNS_parse_packet(unsigned char* data, size_t data_len, bool &ok);
void DNS_free_packet(struct DNS_Packet*);

bool DNS_read_Question(struct DNS_Question*, unsigned char* data, size_t data_len, union pointer*);
bool DNS_read_RR(struct DNS_RR*, unsigned char* data, size_t data_len, union pointer*);
bool DNS_read_name(char* name, unsigned char* data, size_t data_len, union pointer*);

// Given raw packet DNS_req & req_len, craft raw data of DNS Block response: data
void craft_DNS_response(unsigned char* DNS_req, size_t req_len, string TTL, string IP, unsigned char* data);

// Given raw packet DNS_req & req_len, craft modified DNS request, which domain is appended a suffix
void craft_DNS_query(unsigned char* req_buf, size_t req_size, string suffix, unsigned char* data, size_t &len);

// Given Name, get vector of domain/subdmains
vector<string> getSubsFromName(string Name, string& domain);

// Given DNS_packet, get CNAME from it's answer
string getCName(struct DNS_Packet* p);

// Get IP in Answer section of DNS response
string getIP(struct DNS_Packet* p);

// Get ID of DNS request/response
string getID(struct DNS_Packet* p);

// Get URL in Question section of DNS request/response
string getURL(struct DNS_Packet* p);

/*
@ToDo: Do combination logging of IP uni-resp found on CDB

van@monamis:~$ dig @8.8.8.8 www.gmail.com

; <<>> DiG 9.7.3 <<>> @8.8.8.8 www.gmail.com
; (1 server found)
;; global options: +cmd
;; Got answer:
;; ->>HEADER<<- opcode: QUERY, status: NOERROR, id: 5609
;; flags: qr rd ra; QUERY: 1, ANSWER: 5, AUTHORITY: 0, ADDITIONAL: 0

;; QUESTION SECTION:
;www.gmail.com.			IN	A

;; ANSWER SECTION:
www.gmail.com.		86399	IN	CNAME	mail.google.com.
mail.google.com.	86399	IN	CNAME	googlemail.l.google.com.
googlemail.l.google.com. 299	IN	A	173.194.35.55
googlemail.l.google.com. 299	IN	A	173.194.35.54
googlemail.l.google.com. 299	IN	A	173.194.35.53

;; Query time: 254 msec
;; SERVER: 8.8.8.8#53(8.8.8.8)
;; WHEN: Wed Nov 30 07:08:29 2011
;; MSG SIZE  rcvd: 132
*/
// Get list of CNAMEs, inc. orig FQDN {www.gmail.com, mail.google.com, googlemail.l.google.com}
vector<string> getCNames(struct DNS_Packet* p);

// Get list of IPs
vector<string> getIPs(struct DNS_Packet* p);

/*
* For testing purpose
*/
void DNS_dump_packet(struct DNS_Packet*);
void DNS_dump_RR(struct DNS_RR*);

#endif /* __DNS_UTIL_H__ */
