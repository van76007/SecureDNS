/**
 * @file   dns_util.cpp
 * @author dvv <dvv@avauntguard.com>
 * @brief  Implementation of dns_util.h
 */

#include "dns_util.h"

using namespace boost;

/* convert DNS_Type to string */
const char* DNS_Type_str(enum DNS_Type t)
{
    switch (t) {
    case TYPE_A: return "A";
    case TYPE_NS: return "NS";
    case TYPE_MD: return "MD";
    case TYPE_MF: return "MF";
    case TYPE_CNAME: return "CNAME";
    case TYPE_SOA: return "SOA";
    case TYPE_MB: return "MB";
    case TYPE_MG: return "MG";
    case TYPE_MR: return "MR";
    case TYPE_NULL_: return "NULL_";
    case TYPE_WKS: return "WKS";
    case TYPE_PTR: return "PTR";
    case TYPE_HINFO: return "HINFO";
    case TYPE_MINFO: return "MINFO";
    case TYPE_MX: return "MX";
    case TYPE_TXT: return "TXT";
    case TYPE_AAAA: return "AAAA";
    }
    return "UNKNOWN";
}

/* convert DNS_Class to string */
const char* DNS_Class_str(enum DNS_Class c)
{
    switch (c) {
    case CLASS_IN:  return "IN";
    case CLASS_CS:  return "CS";
    case CLASS_CH:  return "CH";
    case CLASS_HS:  return "HS";
    case CLASS_ANY: return "ANY";
    }
    return "UNKNOWN";
}

// Parse DNS req/resp, no need to know the length of data!
// Should know the len of data to prevent mem corruption, meaning check if any pointer jump over the boundary
struct DNS_Packet* DNS_parse_packet(unsigned char* data, size_t data_len, bool &ok)
{
    int i;
    /* allocate memory for the packet */
    struct DNS_Packet* p = (struct DNS_Packet*)malloc(sizeof(struct DNS_Packet));

    /* create a union pointer that points to the begin of data */
    union pointer ptr;
    ptr.uchar = data;

    /* seek through the data and get header fields */
    p->id      = ntohs(*ptr.ushort++);
    p->flags   = ntohs(*ptr.ushort++);
    p->qdcount = ntohs(*ptr.ushort++);
    p->ancount = ntohs(*ptr.ushort++);
    p->nscount = ntohs(*ptr.ushort++);
    p->arcount = ntohs(*ptr.ushort++);

    // printf("p->id: %0#6x, p->flags: %0#6x, p->qdcount: %d, p->ancount: %d, p->nscount: %d, p->arcount: %d\n", p->id, p->flags, p->qdcount, p->ancount, p->nscount, p->arcount);
 
    /* read question */
    if (p->qdcount) {
        /* allocate memory for the question */
        p->question = (struct DNS_Question**)malloc(sizeof(struct DNS_Question*) * p->qdcount);
        for(i = 0; i < p->qdcount; i++) {
            p->question[i] = (struct DNS_Question*)malloc(sizeof(struct DNS_Question));
            ok = DNS_read_Question(p->question[i], data, data_len, &ptr);
            if(!ok)
	    {
	        p->qdcount = i; // Decrease it
                break;
            }
            // printf("Finish reading question[%d]\n", i);
        }
        // printf("Finish reading question ...\n");
    }
	
    /* allocate memory and read answer */
    if (p->ancount) {
        p->answer = (struct DNS_RR**)malloc(sizeof(struct DNS_RR*) * p->ancount);
    	for(i = 0; i < p->ancount; i++) {
        	p->answer[i] = (struct DNS_RR*)malloc(sizeof(struct DNS_RR));
        	ok = DNS_read_RR(p->answer[i], data, data_len, &ptr);
                if(!ok)
		{
		    p->ancount = i; // Decrease it
                    break;
                }
                // printf("Finish reading answer[%d]\n", i);
    	}
        // printf("Finish reading answer ...\n"); 
    }

    /* allocate memory and read authority */
    if (p->nscount) {
        p->authority = (struct DNS_RR**)malloc(sizeof(struct DNS_RR*) * p->nscount);
    	for(i = 0; i < p->nscount; i++) {
        	p->authority[i] = (struct DNS_RR*)malloc(sizeof(struct DNS_RR));
        	ok = DNS_read_RR(p->authority[i], data, data_len, &ptr);
		if(!ok)
                {
		    p->nscount = i; // Decrease it
                    break;
                }
               // printf("Finish reading authority[%d]\n", i);
    	}
        //printf("Finish reading authority ...\n");
    }
	
    /* allocate memory and read additional */
    if (p->arcount) {
    	p->additional = (struct DNS_RR**)malloc(sizeof(struct DNS_RR*) * p->arcount);
    	for(i = 0; i < p->arcount; i++) {
        	p->additional[i] = (struct DNS_RR*)malloc(sizeof(struct DNS_RR));
        	ok = DNS_read_RR(p->additional[i], data, data_len, &ptr);
                if(!ok)
                {
		    p->arcount = i; // Decrease it
		    break;
		}
                //printf("Finish reading additional resource record[%d]\n", i);	
    	}
       // printf("Finish reading additional resource record ...\n");
    }

    /* return pointer to the packet */
    return p;
}

/* free allocated memory of a dns packet */
void DNS_free_packet(struct DNS_Packet* p) {
    int i;

    // printf("\nFree question ...\n");
    for (i = 0; i < p->qdcount; i++)
    {
        // printf("Begin freeing question[%d]\n", i);
        free(p->question[i]);
        // printf("Finish freeing question[%d]\n", i);
    }
    if (p->qdcount)
    	free(p->question);

    // printf("\nFree answer ...\n");
    for (i = 0; i < p->ancount; i++) {
        free(p->answer[i]->rdata);
        free(p->answer[i]);
    }
    if (p->ancount)	
    	free(p->answer);
    
    // printf("\nFree authority ...\n");
    for (i = 0; i < p->nscount; i++) {
        free(p->authority[i]->rdata);
        free(p->authority[i]);
    }
    if (p->nscount)	
    	free(p->authority);
    
    // printf("\nFree additional resource record ...\n");
    for (i = 0; i < p->arcount; i++) {
        free(p->additional[i]->rdata);
        free(p->additional[i]);
    }
    if (p->arcount)	
    	free(p->additional);

    free(p);
}

/* read a question from dns packet and position the pointer behind the end */
bool DNS_read_Question(struct DNS_Question* q, unsigned char* data, size_t data_len, union pointer* ptr)
{
    // printf("Reading question ... \n");

    if (!DNS_read_name(q->name, data, data_len, ptr))
        return false;
    else
	// Name contained withing the question
   	printf("Requested site :: '%s'\n", q->name);
    
    q->dnsType  = DNS_Type(ntohs(*ptr->ushort++));
    q->dnsClass = DNS_Class(ntohs(*ptr->ushort++));
    return true;		
}

/* read a resource record from dns packet and position the pointer behind the end */
bool DNS_read_RR(struct DNS_RR* rr, unsigned char* data, size_t data_len, union pointer* ptr)
{
    union pointer ptrcp;
    bool ok = true;

    if (!DNS_read_name(rr->name, data, data_len, ptr))
        return false;
    
    rr->dnsType   = DNS_Type(ntohs(*ptr->ushort++));
    // printf("after reading DNS type, type is %d\n", rr->dnsType);
    rr->dnsClass  = DNS_Class(ntohs(*ptr->ushort++));
    // printf("after reading DNS class, class is %d\n", rr->dnsClass);
    rr->ttl    = ntohl(*ptr->ulong++);
    rr->length = ntohs(*ptr->ushort++);

    /* get type specific record information */
    switch (rr->dnsType) {
    case TYPE_A:
        rr->rdata = (struct DNS_RDATA_A*)malloc(sizeof(struct DNS_RDATA_A));
        ((struct DNS_RDATA_A*) rr->rdata)->address = *(ptr->in_addr);
        break;
    case TYPE_AAAA:
        rr->rdata = (struct DNS_RDATA_AAAA*)malloc(sizeof(struct DNS_RDATA_AAAA));
        ((struct DNS_RDATA_AAAA*) rr->rdata)->address = *(ptr->in6_addr);
        break;
    case TYPE_CNAME:
        rr->rdata = (struct DNS_RDATA_CNAME*)malloc(sizeof(struct DNS_RDATA_CNAME));
        ptrcp.uchar = ptr->uchar;
        DNS_read_name(((struct DNS_RDATA_CNAME*) rr->rdata)->name, data, data_len, &ptrcp);
        break;
    case TYPE_NS:
        rr->rdata = (struct DNS_RDATA_NS*)malloc(sizeof(struct DNS_RDATA_NS));
        ptrcp.uchar = ptr->uchar;
        DNS_read_name(((struct DNS_RDATA_NS*) rr->rdata)->name, data, data_len, &ptrcp);
        break;
    case TYPE_PTR:
        rr->rdata = (struct DNS_RDATA_PTR*)malloc(sizeof(struct DNS_RDATA_PTR));
        ptrcp.uchar = ptr->uchar;
        DNS_read_name(((struct DNS_RDATA_PTR*) rr->rdata)->name, data, data_len, &ptrcp);
        break;
    case TYPE_SOA:
        rr->rdata = (struct DNS_RDATA_SOA*)malloc(sizeof(struct DNS_RDATA_SOA));
        ptrcp.uchar = ptr->uchar;
        DNS_read_name(((struct DNS_RDATA_SOA*) rr->rdata)->mname, data, data_len, &ptrcp);
        DNS_read_name(((struct DNS_RDATA_SOA*) rr->rdata)->rname, data, data_len, &ptrcp);
        ((struct DNS_RDATA_SOA*) rr->rdata)->serial  = ntohl(*ptrcp.ulong++);
        ((struct DNS_RDATA_SOA*) rr->rdata)->refresh = ntohl(*ptrcp.ulong++);
        ((struct DNS_RDATA_SOA*) rr->rdata)->retry   = ntohl(*ptrcp.ulong++);
        ((struct DNS_RDATA_SOA*) rr->rdata)->expire  = ntohl(*ptrcp.ulong++);
        ((struct DNS_RDATA_SOA*) rr->rdata)->minimum = ntohl(*ptrcp.ulong++);
        break;
    default:
        printf("Default case rr->length is %d\n", rr->length);
        rr->rdata = (struct DNS_RDATA_UNKNOWN*)malloc(sizeof(struct DNS_RDATA_UNKNOWN));
        /*
         * @ToDo: What kind of check we should apply here for memcpy
memcpy()
The function does not check for any terminating null character in source - it always copies exactly num bytes.

To avoid overflows, the size of the arrays pointed by both the destination and source parameters, shall be at least num bytes, and should not overlap (for overlapping memory blocks, memmove is a safer approach).
	*/
        memcpy(((struct DNS_RDATA_UNKNOWN*) rr->rdata)->data, ptr->uchar, rr->length);
        break;
    }

    ptr->uchar += rr->length;

    return ok;
}

/* read a domain name from dns packet and position the pointer behind the end */
// @ToDo: Replace recursive implementation?. What is the length of the name??
// @Note: This is "human-readble" name. In the packet, 0x03 is used instead of '.'. The domain name in question part is 0x03www0x03cnn0x03com0x00
// len can be less than zero as it can read backwards as in case of Additional resource record

// Out: name
// In: data & ptr: Reader pointer
// Idea: reader pointer should not move outside the *data buffer
// May break into 3 cases: reading domain, answer and additional resource record

bool DNS_read_name(char* name, unsigned char* data, size_t data_len, union pointer* ptr)
{
    char* run = name;
    bool ok = true;   
 
    while(1) {
        /* read one byte */
        char len = *((ptr->uchar)++);	
        
        // @ToDo: Add this check for length here and return smt like NULL
        if (len == 0) { /* no labels left */
            *(run--) = 0; // trick to remove the last dot we added in programming
            break;
        } 
        else if ((len & 0xc0) == 0xc0) {
            /* this is a pointer to a previous domain name, reading backward */
            union pointer subptr;

            /* remove the first bits from the len-byte and bitwise-or it with
               the following byte to get the offset, add it to the data pointer
               to get a pointer to the previous domain name */
            //subptr.uchar = data + ((len & 0x3f) << 8 | *((ptr->uchar)++));
            /* should compare this distance with len of data and bail out if distance is too large */
            size_t dist = (len & 0x3f) << 8 | *((ptr->uchar)++);
            /* Must validate this pointer: Shifting distance should not more than boundary. Data is constant and so the data length */
            // printf("shifting distance is %d\n", dist);
            if((dist<0) || (dist>data_len)) {
                return false;
            }

            subptr.uchar = data + dist;
            
            // Recursive call. Or we can read directly here base on distance
            ok = ok && DNS_read_name(run, data, data_len, &subptr);
            return ok;
        }
        else {
            // Case: Reading domain name of Question section
            if((len<0) || (len>data_len)) {
                return false;
            }
 
            /* read len bytes */
            int i;
            for(i = 0; i < len; i++)
	    {
                *(run++) = *((ptr->uchar)++);	
	    }
            *(run++) = '.';
        }
    }    

    if (run == name) /* empty name will be '.' by default */
        *(run++) = '.';

    *run = 0;
    return ok;
}


// Given raw data of DNS_Request, craft raw data of DNS Block response: data and output len as well
// Caller can pre-alloc data[512] and pass it to the callee
// Should not use the struct DNS_Packet as the name is human-readable string rather than encoded string
// Configurable parameters: TTL and IP_of_blocked_page readed as string from config file 
void craft_DNS_response(unsigned char* DNS_req, size_t req_len, std::string TTL, std::string IP, unsigned char* data) {
    
    // Copy request to response
    memcpy(data, DNS_req, req_len);
    // printf("Passing memcpy\n");
    
    /* for (int i = 0; i < req_len; i++)
        printf("%02x ", data[i]);
    printf("\n"); */

    // Twist header section
    *(data+2) = 0x81; // flags is harded code 81 80
    *(data+3) = 0x80;
    *(data+6) = 0x00; // #ancount 00 01
    *(data+7) = 0x01; // is 1
    // printf("Passing twist header section\n");

    // Append IP record
    *(data + req_len) = 0xc0; // Name
    *(data + req_len + 1) = 0x0c;
    *(data + req_len + 2) = 0x00; // Type A 00 01
    *(data + req_len + 3) = 0x01;
    *(data + req_len + 4) = 0x00; // Class IN 00 01
    *(data + req_len + 5) = 0x01;
    // printf("Passing append IP record\n");

     // TTL in . format
     tokenizer<> tokTTL(TTL);
     int i = 0; // @ToDo: use reader pointer to advance reading. Converting is WRONG 
     for(tokenizer<>::iterator beg=tokTTL.begin(); beg!=tokTTL.end();++beg){
        *(data + req_len + 6 + i) = static_cast<unsigned char>(atoi((*beg).c_str()));
        i++;	
     }
     // printf("Passing append TTL\n");
	
     // Data length, i.e. length of IP
     *(data + req_len + 10) = 0x00;
     *(data + req_len + 11) = 0x04;
     // printf("Passing append IP length\n");	
    
     // IP. Must check first if compliant to . format
     tokenizer<> tokIP(IP);
     int j = 0;
     for(tokenizer<>::iterator beg=tokIP.begin(); beg!=tokIP.end();++beg){
         *(data + req_len + 12 + j) = static_cast<unsigned char>(atoi((*beg).c_str()));
         j++;		
     }	
     // printf("\nPassing append IP\n");		
}

void craft_DNS_query(unsigned char* req_buf, size_t req_size, std::string suffix, unsigned char* data, size_t &len) {
    
    // Parse packet to get domain
    bool ok = true;

    struct DNS_Packet* p = DNS_parse_packet(req_buf, req_size, ok);
    
    if (!ok) {
        printf("\nMalformed packet\n");
        DNS_free_packet(p);
        return; 
    }	
    DNS_dump_packet(p);
    
    /* 
    domain: Convert from human readble format to DNS format and Append .suffix to domain
    question->name = ""www.vnn.vn" -> "77 77 77 2e 76 6e 6e 2e 76 6e"
    "multi.surbl.org" -> 6d 75 6c 74 69 2e 73 75 72 62 6c 2e 6f 72 67
    03 is the length of the string follow dot
    Result: 03 | 77 77 77 '03' 76 6e 6e '03' 76 6e | 03 | 6d 75 6c 74 69 '03' 73 75 72 62 6c '03' 6f 72 67 | 00	
    */

    // Craft a new DNS querry from header section and new Question section
    // Copy request's header to crafted request
    memcpy(data, req_buf, 12); // Size of DNS packet's header: 12

    // Twist the query's ID
    srand(time(NULL));
    unsigned short id = rand() % 65536;
    *data = (id & 0xff00) >> 8;       
    *(data + 1) = id & 0x00ff;

    // Craft the Question section. For now, 1 question only, not multicast
    // 12 is the size of Header section. 0x2e == '.'
    int inc = 0;
    if (p->qdcount) {
        // For each question
        for (int i = 0; i < p->qdcount; i++)
        {      
	    int lock = 0;
            // Append orig domain
            strcat(p->question[i]->name, "."); // Trick to append . at the end of the string 
	    for (int j = 0; j < strlen(p->question[i]->name) + 1; j++)
            {
                if (p->question[i]->name[j] == 0x2e) {
		    *(data + 12 + inc) = j-lock;
      		    inc++;

      		    for(;lock<j;lock++) {
		        *(data + 12 + inc) = p->question[i]->name[lock];
        	        inc++;
      		    }
      		    lock++;
		}
	    }
            
	    // Append RBL domain
            lock = 0;
            suffix.append("."); // Trick to append . at the end of the string
            const char* p = suffix.c_str(); 
	    for (int k = 0; k < suffix.length(); k++)
            {
		if (*(p+k) == 0x2e) {
                    *(data + 12 + inc) = k-lock;
      		    inc++;

      		    for(;lock<k;lock++) {
		        *(data + 12 + inc) = *(p+lock);
        	        inc++;
      		    }
      		    lock++;
                }  
            }
            
	    *(data + 12 + inc) = 0x00; inc++; // End of craft domain in craft question
	    *(data + 12 + inc) = 0x00; inc++; // Class in craft question 
            *(data + 12 + inc) = 0x01; inc++;
    	    *(data + 12 + inc) = 0x00; inc++; // Type in craft question 
            *(data + 12 + inc) = 0x01; inc++;
        }
    }	
    
    // Update the len of new data buffer accordingly
    len = 12 + (size_t)inc;
       
    DNS_free_packet(p);	 
}

/* dump a packet to stdout */
void DNS_dump_packet(struct DNS_Packet* p) {
    int i;
   /* printf(";; HEADER: \n");
    printf("  id: %0#6x, flags: %0#6x\n"
           "  qdcount: %d, ancount: %d, nscount: %d, arcount: %d\n",
        p->id,
        p->flags,
        p->qdcount,
        p->ancount,
        p->nscount,
        p->arcount);

    if (p->qdcount) {
        printf("\n");
        printf(";; QUESTION:\n");
        for (i = 0; i < p->qdcount; i++)
        {
           printf("%19s            %3s %5s name_len is %u\n",
                p->question[i]->name,
                DNS_Class_str(p->question[i]->dnsClass),
                DNS_Type_str(p->question[i]->dnsType), (unsigned)strlen(p->question[i]->name));
            // Test
            // "www.vnn.vn" -> "77 77 77 2e 76 6e 6e 2e 76 6e", no end by zero and no start by 03
          //  printf("Domain name in hex string ...\n");
            for (int j = 0; j < strlen(p->question[i]->name); j++)
                printf(" %02x", p->question[i]->name[j]);
	    printf("_\n");	
        }
    }
    if (p->ancount) {
      //  printf("\n");
      //  printf(";; ANSWER:\n");
        for (i = 0; i < p->ancount; i++)
            DNS_dump_RR(p->answer[i]);
    }
    if (p->nscount) {
        printf("\n");
        printf(";; AUTHORITY:\n");
        for (i = 0; i < p->nscount; i++)
            DNS_dump_RR(p->authority[i]);
    }
    if (p->arcount) {
        printf("\n");
        printf(";; ADDITIONAL:\n");
        for (i = 0; i < p->arcount; i++)
            DNS_dump_RR(p->additional[i]);
    }*/
}

/* dump a resource record to stdout */
void DNS_dump_RR(struct DNS_RR* rr) {

    printf("name=%19s ttl=%10lu class=%3s type=%5s ",
        rr->name,
        rr->ttl,
        DNS_Class_str(rr->dnsClass),
        DNS_Type_str(rr->dnsType));

    switch (rr->dnsType) {
    case TYPE_A:
	char ipv4[INET_ADDRSTRLEN];        
	inet_ntop(AF_INET, &(((struct DNS_RDATA_A*) rr->rdata)->address), (char*)&ipv4, INET_ADDRSTRLEN);
        printf("ip=%s", ipv4);
        break;
    case TYPE_AAAA:
	char ipv6[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &(((struct DNS_RDATA_AAAA*) rr->rdata)->address), (char*)&ipv6, INET6_ADDRSTRLEN);
        printf("ip=%s", ipv6);
        break;
    case TYPE_CNAME:
        printf("%s", ((struct DNS_RDATA_CNAME*) rr->rdata)->name);
        break;
    case TYPE_NS:
        printf("%s", ((struct DNS_RDATA_NS*) rr->rdata)->name);
        break;
    case TYPE_PTR:
        printf("%s", ((struct DNS_RDATA_PTR*) rr->rdata)->name);
        break;
    case TYPE_SOA:
        printf("%s %s %lu %lu %lu %lu %lu",
            ((struct DNS_RDATA_SOA*) rr->rdata)->mname,
            ((struct DNS_RDATA_SOA*) rr->rdata)->rname,
            ((struct DNS_RDATA_SOA*) rr->rdata)->serial,
            ((struct DNS_RDATA_SOA*) rr->rdata)->refresh,
            ((struct DNS_RDATA_SOA*) rr->rdata)->retry,
            ((struct DNS_RDATA_SOA*) rr->rdata)->expire,
            ((struct DNS_RDATA_SOA*) rr->rdata)->minimum);
        break;
    default:
        printf("Unknown");
    }

    printf("\n");
}
