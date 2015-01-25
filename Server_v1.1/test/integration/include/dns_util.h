/**
 * @file   dns_util.h
 * @author dvv <dvv@avauntguard.com>
 * @brief  Model structure of DNS request/response & provide parsing DNS packets functionalities
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
#include<boost/tokenizer.hpp> // To split string for IP

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
    unsigned long ttl;  
    unsigned long length;
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
    unsigned long* ulong;

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
void craft_DNS_response(unsigned char* DNS_req, size_t req_len, std::string TTL, std::string IP, unsigned char* data);

// Given raw packet DNS_req & req_len, craft modified DNS request, which domain is appended a suffix
void craft_DNS_query(unsigned char* req_buf, size_t req_size, std::string suffix, unsigned char* data, size_t &len);

/*
* For testing purpose
*/
void DNS_dump_packet(struct DNS_Packet*);
void DNS_dump_RR(struct DNS_RR*);

#endif /* __DNS_UTIL_H__ */
