/**
 * author SNU and DCO
 * @file
 * Part of Code taken from http://www.binarytides.com/blog/dns-query-code-in-c-with-winsock-and-linux-sockets/
 * @brief Implementation of DNSstructure
 *	
 * @section Description
 * This file contains the implementation of the DNSstructure
 * singleton.
 * @TODO ensure that we throw more useful information 
 */

#include"DNSstructure.h"
#include <boost/thread.hpp>
#include <stddef.h>
#include <fstream>
#include <fcntl.h>
#include <DNSList.h>
#include <iostream>
#include <sstream>

extern std::string IpName;
extern std::string PortName;
extern std::string blPg;
extern std::string ttl;

// Init static variable to NULL
Nameserver::DNSstructure* Nameserver::DNSstructure::mpsInstance = NULL;

// Constructor private (Singleton)
Nameserver::DNSstructure::DNSstructure()
{
}; //DNSstructure

// Private
Nameserver::DNSstructure::~DNSstructure() 
{
}; //~DNSstructure

// Static
Nameserver::DNSstructure* Nameserver::DNSstructure::GetInstance()
{
  if(mpsInstance == NULL) {
    mpsInstance = new Nameserver::DNSstructure();
  }
  return mpsInstance;
}; //GetInstance

/* We use a special SOCKET type for easier Windows porting */
#define SOCKET int

//Type field of Query and Answer
#define T_A 1 /* host address */
#define T_NS 2 /* authoritative server */
#define T_CNAME 5 /* canonical name */
#define T_SOA 6 /* start of authority zone */
#define T_PTR 12 /* domain name pointer */
#define T_MX 15 /* mail routing information */

// DNS header structure
struct DNS_HEADER
{
  unsigned short id; 		// identification number

  unsigned char rd :1; 		// recursion desired
  unsigned char tc :1; 		// truncated message
  unsigned char aa :1; 		// authoritive answer
  unsigned char opcode :4; 	// purpose of message
  unsigned char qr :1; 		// query/response flag

  unsigned char rcode :4; 	// response code
  unsigned char cd :1; 		// checking disabled
  unsigned char ad :1; 		// authenticated data
  unsigned char z :1; 		// its z! reserved
  unsigned char ra :1; 		// recursion available

  unsigned short q_count; 	// number of question entries
  unsigned short ans_count; 	// number of answer entries
  unsigned short auth_count; 	// number of authority entries
  unsigned short add_count; 	// number of resource entries
};

// Constant sized fields of query structure
struct QUESTION
{
  unsigned short qtype;
  unsigned short qclass;
};

// Constant sized fields of the resource record structure
#pragma pack(push, 1)
struct R_DATA
{
  unsigned short type;
  unsigned short _class;
  unsigned int ttl;
  unsigned short data_len;
};
#pragma pack(pop)

typedef struct 
{
  unsigned short num;
}TESTNUM;


/* This is the header placed before the 4-byte IP; we change the last four
 * bytes to set the IP we give out in replies */
char p[17] = "\xc0\x0c\x00\x01\x00\x01\x00\x00\x00\x00\x00\x04\x7f\x7f\x7f\x7f";

//int write2lg( std::string domain, std::string client_ip, std::string response, std::string zoneName);
int write2lg( std::string completeDom, std::string domain, std::string client_ip, std::string response, std::string zoneName);

struct RES_RECORD
{
  unsigned char* name;
  struct R_DATA *resource;
  unsigned char rdata[100];
};



// Structure of a Query
typedef struct
{
  unsigned char* name;
  struct QUESTION *ques;
} QUERY;


std::string readname(unsigned char* reader,unsigned char* buffer,int* count)
{
  //unsigned char *name;
  unsigned int p1=0,jumped=0,offset;
  int i, j;
  *count = 1;
  //name = (unsigned char*)malloc(256 * sizeof(unsigned char));
  unsigned char name[512];
  memset (&name,'\0',512);
  name[0]=' ';
  //read the names in 3www6google3com format
  while(*reader!=0)
    {
      if(*reader>=192)
	{
	  offset = (*reader)*256 + *(reader+1) - 49152; //49152 = 11000000 00000000
	  reader = buffer + offset - 1;
	  jumped = 1; //we have jumped to another location so counting wont go up!
	}
      else{
	name[p1++]=*reader;
      }
      reader=reader+1;
      if(jumped==0)
	*count = *count + 1; //if we havent jumped to another location then we can count up
    }
  if(jumped==1)
    *count = *count + 1; //number of steps we actually moved forward in the packet
  //now convert 3www6google3com0 to www.google.com
  int end = p1;
  for(i=0;i < end /*(int)strlen((const char*)name)*/;i++) {
    p1=name[i];
    for(j=0;j<(int)p1;j++) {
      name[i]=name[i+1];
      i=i+1;
    }
    name[i]='.';
  }
  name[end-1]='\0'; //remove the last dot
  std::stringstream s;
  s << name;
  std::string nameStr = s.str();
  return nameStr;
}

std::string findName(int sizeQuery, const char* CompleteQuery, int start){

  int index = 0;
  char domain[DNSLENGTH];
  for(int i = start; i < sizeQuery; i++) {
    int word_length = CompleteQuery[i];
    if(word_length <= 0) {				
      break;
    }
    else {
      int max = i + word_length;
      if(i != start){
#ifdef DEBUG
	printf(".");  
#endif
	domain[index] = '.'; 
	index++;			
      }                                                   
      for(int j = i+1; j <= max; j++){
	domain[index] = CompleteQuery[j];
	i++;
	index++;
      }                                                             
    }
  }     
  domain[index] = '\0';
  std::string strdomain(domain);
  return strdomain;
}

void Nameserver::DNSstructure::QuickTestCNIP(int size_request, 
					     unsigned char* response_buffer, 
					     int bytecount, 
					     std::vector<std::string>& IPLi, 
					     std::string* RealHostname){
  int ref_pt = size_request;
  while(ref_pt+12< bytecount){
    unsigned char buf[2];
    buf[0] = response_buffer[ref_pt + 10];buf[1] = response_buffer[ref_pt + 11];
    TESTNUM *tp = NULL;
    tp = (TESTNUM*) buf;
   
    int index = 0;
    char domain[DNSLENGTH];
    int inttest = int(response_buffer[ref_pt + 3]);
    if (inttest != 1 && inttest != 5) break;
    if(inttest == 5){
      for(int i = ref_pt+12; i < ref_pt + htons( tp->num )+12 ; i++) {
	if(i > bytecount-1) return;
	int word_length = response_buffer[i];	
	if(word_length == 192) {//should be greater than
	  if(index >bytecount) return;
	  domain[index] = '.'; 
	  index++;
	  printf("\n");
	  int w = 0;
	  int ptr = (int)response_buffer[i+1];  
	  int h = 0;
	  while(1){
	    if(ptr > bytecount)return;
	    w = response_buffer[ptr];
	    if(w > 191 || w < 1) break;
	    for(h = ptr + 1; h < ptr+1+w; h++ ){
	      if(index >bytecount)return; 
	      domain[index] = response_buffer[h];
	      index++;	
	    }
	    domain[index] = '.';
	    index++;
	    ptr = h;
	  }
	  domain[index - 1] = '\0';
	  break;		
	}
	if(word_length <= 0){				
	  break;
	}
	else {
	  int max = i + word_length;
	  if(i != ref_pt+12){
	    if(index >bytecount)break;
	    domain[index] = '.'; 
	    index++;			
	  } 
	  for(int j = i+1; j <= max; j++){
	    if(j >bytecount || index >bytecount)return;
	    domain[index] = response_buffer[j];
	    i++;
	    index++;
	  }                                                             
	}
      }     
      domain[index] = '\0';
      std::string strdomain(domain);
      ref_pt = ref_pt + htons( tp->num) + 12;
      *RealHostname = strdomain;
    }
    std::string ip1 = "";
    if(inttest == 1){//IP address
      for(int i = ref_pt+12; i < ref_pt + htons( tp->num )+12 ; i++) {
	//std::cout << "wl: " << wl << std::endl;
	int ipmax = i + htons( tp->num );
	if(i>bytecount)return;
	int ip = (int)response_buffer[i];
	std::string sip;
	std::stringstream out;
	out << ip;
	sip = out.str();
	ip1 += sip;
	if (i != ref_pt + htons( tp->num )+11)
	  ip1 += ".";
      }
      ref_pt = ref_pt + htons( tp->num) + 12;
      if(IPLi.size() > 7 ) IPLi.clear();
      IPLi.push_back(ip1);
    }
  }	
}

void Nameserver::DNSstructure::ListCNIP(const char* request_buffer, unsigned char* response_buffer, int bytecount, std::vector<std::string>& IPLi, std::string* RealHostname)
{
  struct RES_RECORD answers[20]; //the replies from the DNS server
  struct DNS_HEADER *dns = NULL;
  //struct QUESTION *qinfo = NULL;
  unsigned char *reader, *qname; 
  qname =(unsigned char*)&request_buffer[sizeof(struct DNS_HEADER)];
  dns = (struct DNS_HEADER*) response_buffer;
  // move ahead of the dns header and the query field
  reader = &response_buffer[sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION)];
  printf("Response length is %d\n", bytecount);
  printf("*************************************\n");
  printf("* The response contains : \n");
  printf("*************************************\n");
  printf("* %d Questions.\n",ntohs(dns->q_count));
  printf("* %d Answers.\n",ntohs(dns->ans_count));
  printf("* %d Authoritative Servers.\n",ntohs(dns->auth_count));
  printf("* %d Additional records.nn\n",ntohs(dns->add_count));
  printf("*************************************\n");
  //reading answers
  int stop = 0;
  for(int i=0;i<ntohs(dns->ans_count);i++)
    {
      std::string ansName(readname(reader,response_buffer,&stop));
      unsigned char TempAnsName[512];
      memset (&TempAnsName,'\0',512);
      strcpy( (char*)TempAnsName, ansName.c_str() );
      answers[i].name = TempAnsName;

      //answers[i].name = readname(reader,response_buffer,&stop);
      reader = reader + stop;
      answers[i].resource = (struct R_DATA*)(reader);
      reader = reader + sizeof(struct R_DATA);
      if(ntohs(answers[i].resource->type) == 1) //if its an ipv4 address
	{
	  //printf("DataLen:-%i",ntohs(answers[i].resource->data_len));
	  if(ntohs(answers[i].resource->data_len) > 100 ) return;			
	  //answers[i].rdata = (unsigned char*)malloc(ntohs(answers[i].resource->data_len));
	  for(int j=0 ; j<ntohs(answers[i].resource->data_len) ; j++)
	    answers[i].rdata[j]=reader[j];
	  answers[i].rdata[ntohs(answers[i].resource->data_len)] = '\0';//' ';
	  reader = reader + ntohs(answers[i].resource->data_len);
	}
      else
	{
	  std::string ansData( readname(reader,response_buffer,&stop) );
	  unsigned char TempAnsData[512];
	  memset (&TempAnsData,'\0',512);
	  std::cout << "STRLEN ansDATA: " << strlen(ansData.c_str()) << std::endl;
	  if( ntohs( strlen(ansData.c_str()) > 100) ) return;
	  strcpy( (char*)TempAnsData, ansData.c_str() );
	  std::cout << "ans" << std::endl;
	  for(int h = 0; h < int(strlen(ansData.c_str())); h++ )
	    answers[i].rdata[h] = TempAnsData[h];//readname(reader,response_buffer,&stop);
	  answers[i].rdata[int(strlen(ansData.c_str()))] = '\0';
	  reader = reader + stop;
	}
    }
			
  struct sockaddr_in a;
  std::cout << "Beofre dns ans:" << ntohs(dns->ans_count) << std::endl;
  //print answers
  for(int i=0;i<ntohs(dns->ans_count);i++)
    {
      std::cout <<  "i is: " << i << std::endl;
      if(ntohs(answers[i].resource->type)==1) //IPv4 address
	{
	  std::cout << "*P" << std::endl;
	  //
	  std::cout << "Casting to long*" << std::endl;
	  long *p=(long*)answers[i].rdata;
	  std::cout << "passing pointer" << std::endl;
	  a.sin_addr.s_addr=(*p); //working without ntohl
	  std::cout << "a.sin_addr:" << inet_ntoa(a.sin_addr) << std::endl;
	  IPLi.push_back(inet_ntoa(a.sin_addr));
	  std::cout << "IPli.size(): " << IPLi.size() << std::endl;
	  for(int k=0; k < IPLi.size(); k++){
	    std::cout << "IPLi: " << IPLi[k] << std::endl;
	  }
	}
      std::cout << "answers[i].resource->type: " << answers[i].resource->type << std::endl;
      if(ntohs(answers[i].resource->type)==5) //Canonical name for an alias
	{
	  std::cout << "(char*)answers[i].rdata: " << (char*)answers[i].rdata << std::endl;
	  std::string cn((char*)answers[i].rdata);
	  std::cout <<  "cn: " << cn << std::endl;
	  *RealHostname = cn;
	  std::cout << "Finished with cn" << std::endl;
	}
    }
}



//not using this function to create the connection
MYSQL* createcon(std::string server, std::string user, std::string password, std::string database)
{
  MYSQL* conn = mysql_init(NULL);
  /* Connect to database */
  if (!mysql_real_connect(conn, server.c_str(), user.c_str(), password.c_str(), database.c_str(), 0, NULL, 0)) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    mysql_close(conn);
  }
  return conn;
}//end create connection

//function to log the blocked IP and domains to database
//int write2lg( std::string domain, std::string client_ip, std::string response, std::string zoneName) 
int write2lg( std::string compleDom, std::string domain, std::string client_ip, std::string response, std::string zoneName)
{
  int res;
  res = open("BlockedDomain", O_WRONLY);
  if(res < 0) return 1;
  std::string sent = compleDom;
  sent += "(";
  sent += domain;
  sent += "(";
  sent += client_ip;
  sent += "(";
  sent += response;
  sent += "(";
  sent += zoneName;
  sent += "\n";
  write(res,sent.c_str(),strlen(sent.c_str())); 
  close(res);
  return 1;
}//end write2lg


//this will convert www.google.com to 3www6google3com
void Nameserver::DNSstructure::ChangetoDnsNameFormat( char* dns, std::string host) {
  int lock = 0;
  char hostbuf[DNSLENGTH];
  strcpy(hostbuf, host.c_str());
  strcat(hostbuf,".");
  for(  int i = 0 ; i < (int)strlen(host.c_str()) + 1  ; i++) {
    if(hostbuf[i]=='.') {
      *dns++=i-lock;
      for(;lock<i;lock++) {
	*dns++=hostbuf[lock];
      }
      lock++;
    }
  }
#ifdef DEBUG
  std::cout << "HOST: "  << hostbuf << std::endl;
#endif
  *dns++=' ';
}

//determine the IP from the response
std::string DetIPResponse( char* resp_buf, int bytecnt){
  //determine the IP from the response
  char ip[15];
  char completeIP[15];
  if(bytecnt < 5) return "Could not DetIPResponse";
  sprintf(ip, "%i", resp_buf[bytecnt-4]);
  strcpy(completeIP,ip);
  strcat(completeIP, ".");
  for(int i = bytecnt - 3; i < bytecnt; i++){
    sprintf(ip, "%i", resp_buf[i]);
    strcat(completeIP, ip);
    if( i != bytecnt - 1)
      strcat(completeIP, ".");
  }
  std::string IPResponse(completeIP);
#ifdef DEBUG
  std::cout << "Last UP is : " <<  completeIP << std::endl;
#endif
  return IPResponse;

}

//create the query with the modified 
int Nameserver::DNSstructure::CreateQuery( int MainQuerySize, 
					   std::string MainQuery_buffer, 
					   std::string IPNameServ,
					   int PortNameServ,
					   std::string hostname, 
					   std::string Origdomain,
					   int MainIncoming_Socket, 
					   struct sockaddr_in Mainlocal_info,
					   unsigned int Mainsocklen)
{
#ifdef DEBUG
  std::cout << "RBL Lookup" << std::endl;
#endif
  int Blvect = 0; 
#ifdef DEBUG
  std::cout << "MainQuery_buffer" << hostname << std::endl;
#endif	
  char buf1[DNSLENGTH], response_buffer[DNSLENGTH], *qname1;
  int i, s1;
  memset(&buf1, '\0', DNSLENGTH);
  memset(&response_buffer, '\0', DNSLENGTH);
  struct sockaddr_in a1;
  struct sockaddr_in dest1;
  unsigned int socklen1 = sizeof(a1); 

#ifdef DEBUG
  for(int k = 0; k<30;k++)
    printf("bef.......:%02x", buf1[k]);
  printf("\n");
#endif
	 
  struct DNS_HEADER *dns1 = NULL;
  struct QUESTION *qinfo1 = NULL;
#ifdef DEBUG
  printf("Looking up : ");
  for (int ch = 0; hostname.c_str()[ch] != '\0'; ++ch) { //keep going until we hit the '\0'
    std::cout  << hostname.c_str()[ch]; //print a single character and jump to the next line}
  }	 
#endif
  s1 = socket( AF_INET , SOCK_DGRAM , 0 );
  if(-1 == s1) {//UDP packet for DNS queries
    perror("Error Creating Socket!"); 	
    Blvect = 2;
    return 1;
  }

  int option_value = 1;
  if(0 != setsockopt(s1, SOL_SOCKET, SO_REUSEADDR, (void*)&option_value, sizeof(option_value))){
    std::cout <<  "Error S1 setsocket" << std::endl;
    close(s1);
    return 1;
  }
  memset(&a1, '\0', sizeof(struct sockaddr_in));
  dest1.sin_family = AF_INET;
  dest1.sin_port = htons(PortNameServ);
  dest1.sin_addr.s_addr = inet_addr(IPNameServ.c_str()); //dns servers

  /*Set socket to NonBlocking*/
  int x;
  x=fcntl(s1,F_GETFL,0);
  fcntl(s1,F_SETFL,x | O_NONBLOCK);
  /**/	 
  unsigned short idn;
  //Set the DNS structure to standard queries
  dns1 = (struct DNS_HEADER *)&buf1;
  srand(time(NULL));
  idn = rand() % 65536; //previously had a static TID but need to be random
  dns1->id = idn;//(unsigned short) htons(getpid());
  dns1->qr = 0; //This is a query
  dns1->opcode = 0; //This is a standard query
  dns1->aa = 0; //Not Authoritative
  dns1->tc = 0; //This message is not truncated
  dns1->rd = 1; //Recursion Desired
  dns1->ra = 0; //Recursion not available! hey we dont have it (lol)
  dns1->z = 0;
  dns1->ad = 0;
  dns1->cd = 0;
  dns1->rcode = 0;
  dns1->q_count = htons(1); //we have only 1 question
  dns1->ans_count = 0;
  dns1->auth_count = 0;
  dns1->add_count = 0;
#ifdef DEBUG
  for(int k = 0; k<30;k++)
    printf("buf.......:%02x", buf1[k]);
  printf("\n");
#endif
  //point to the query portion
  qname1 = &buf1[sizeof(struct DNS_HEADER)];
#ifdef DEBUG 
  std::cout << "hostname: " << hostname << "\\" << std::endl;
#endif
  ChangetoDnsNameFormat(qname1 , hostname);
  qinfo1 =(struct QUESTION*)&buf1[sizeof(struct DNS_HEADER) + (strlen((const char*)qname1) + 1)]; //fill it	 
  int start_pointer = sizeof(struct DNS_HEADER) + strlen((const char*)qname1);
  // Sets the five last bytes of the packet
  buf1[start_pointer - 1] = 0;
  buf1[start_pointer++] = 0;
  buf1[start_pointer++] = 1;
  buf1[start_pointer++] = 0;
  buf1[start_pointer++] = 1;

  if(sendto(s1,buf1, sizeof(struct DNS_HEADER) + (strlen((const char*)qname1) + 1) + sizeof(struct QUESTION),0,(struct sockaddr*)&dest1, sizeof(dest1)) == 0){
    std::cout << "Could not send the blocked request" << std::endl;
    Blvect = 2;
    close(s1);
    // close(MainIncoming_Socket);
    return 1;
  }
  i = sizeof dest1;
  int bytecount = 0;
  struct timeval timeout;
  fd_set fds1;
  FD_ZERO(&fds1);
  FD_SET(s1, &fds1);
  //a timeout of only one second
  timeout.tv_sec = 2;
  timeout.tv_usec = 0;
  int rc = select(s1 + 1, &fds1, NULL, NULL, &timeout);
  if( -1 == rc){
    std::cout << "Could not send the blocked request" << std::endl;
    Blvect = 2;
    close(s1);
    // close(MainIncoming_Socket);
    return 1;
  }
  if (rc > 0){
    if(FD_ISSET(s1, &fds1)){
      if((bytecount = recvfrom(s1, response_buffer, DNSLENGTH, 0,(struct sockaddr*)&dest1, &socklen1)) <= 0){
	std::cout << "Could not send the blocked request" << std::endl;
	Blvect = 2;
	close(s1);
	//close(MainIncoming_Socket);
	return 1;
      }
    }
  }	
#ifdef DEBUG
  std::cout << "Finished Receiving data for the RBLs: bytes" << bytecount << std::endl;
#endif
#ifdef DEBUG
  //we check first the most significant bit of the answer to determine if a page is blocked
  std::cout << "bytecount: " << bytecount << std::endl; 
  for(int i = 0; i < bytecount; i++){
    printf( "-%02x", response_buffer[i] );	
  }	
#endif
  Nameserver::DNSList* mDNSList = Nameserver::DNSList::GetInstance();
  // unsigned char modifiedResponse[DNSLENGTH];
  if( response_buffer[7] > 0 || response_buffer[6] > 0){
    //determine the IP from the response
    // setIPResponse(DetIPResponse(response_buffer, sizeof(struct DNS_HEADER) + (strlen((const char*)qname1) + 1) + sizeof(struct QUESTION) + 16));
    std::string IPres = DetIPResponse(response_buffer, sizeof(struct DNS_HEADER) + (strlen((const char*)qname1) + 1) + sizeof(struct QUESTION) + 16);
    std::vector<std::string> spIP;
    std::string ipres = GetIPResponse();
    
    /*    boost::split(spIP, ipres, boost::is_any_of("."));
	  if(0 != strcmp( spIP[0].c_str(), "127")) {
	  Blvect = 2;
	  std::cout << "Strange IP mismatched" << std::endl;
	  close(s1);
	  //	close(MainIncoming_Socket);
	  return 1;	
	  };
    */    
    std::string zoneName = hostname.substr( strlen(Origdomain.c_str()) + 1, strlen(hostname.c_str()) - strlen(Origdomain.c_str()));
    //mDNSList->setZoneName(hostname.substr( strlen(Origdomain.c_str()) + 1, strlen(hostname.c_str()) - strlen(Origdomain.c_str()) ));
    mDNSList->setZoneName(zoneName);
    CreateBlockedResponse( MainQuery_buffer, 
			   MainQuerySize,  
			   MainIncoming_Socket,
			   Mainlocal_info,
			   Mainsocklen, 
			   Origdomain );
   
   
    //getpeername(MainIncoming_Socket, (struct sockaddr*)&Mainlocal_info, &Mainsocklen);
    //std::string clienltIPAdd(inet_ntoa(Mainlocal_info.sin_addr));
#ifdef DEBUG
    std::cout << "IPRBL: " << GetIPResponse() << std::endl;
    std::cout << "ZoneName: " << GetZoneName() << std::endl;  
#endif
    //currently having both argument the same will do but when we later need sudomains we will change it here
    write2lg( Origdomain, Origdomain, "Disabled", IPres, mDNSList->GetZoneName()) ;
#ifdef DEBUG
    std::cout << "Before return in CreateQuery - Blocked" << std::endl;
#endif
    Blvect = 1;
    close(s1);
    return 1;
  }
#ifdef DEBUG
  std::cout << "Did not find a blocked IP" << std::endl;
#endif
  Blvect = 2;
  close(s1);
  return 1;
} 

//function to create the response once the query has been identified as being blocked
int Nameserver::DNSstructure::CreateBlockedResponse( std::string response_array, 
						     int numberReceived, 
						     int main_socket,
						     struct sockaddr_in local_info,
						     unsigned int socklen, 	 
						     std::string domainName ){
  char DNSResponse[DNSLENGTH];
#ifdef DEBUG
  std::cout << "Number Received: " << numberReceived<< std::endl;
#endif
  //copy the whole array to the 
  memcpy(DNSResponse, response_array.c_str(), numberReceived );
#ifdef DEBUG
  for(int i = 0; i < 30; i++){
    printf( ":%02x", DNSResponse[i] );	
  }
  printf("\n");
#endif
  //Modify the 3rd and 4th bytes of the DNS query 
  DNSResponse[2] = 129;
  DNSResponse[3] = 128;
  //Modify the answer- Only one answer
  DNSResponse[6] = 0;
  DNSResponse[7] = 1;
  //Ending with C0 and 0C
  *(DNSResponse + numberReceived) = 192;
  *(DNSResponse + numberReceived+1) = 12;
  //Type
  *(DNSResponse + numberReceived+2) = 0;
  *(DNSResponse + numberReceived+3) = 1;
  //Class
  *(DNSResponse + numberReceived+4) = 0;
  *(DNSResponse + numberReceived+5) = 1;
  //TTL
  std::vector<std::string> BreakTTLvector;
  boost::split(BreakTTLvector, ttl, boost::is_any_of("."));
  int FirstPartTTL = boost::lexical_cast<int>(BreakTTLvector[0]);
  *(DNSResponse + numberReceived+6) = FirstPartTTL;
  int SecondPartTTL = boost::lexical_cast<int>(BreakTTLvector[1]);
  *(DNSResponse + numberReceived+7) = SecondPartTTL;
  int ThirdPartTTL = boost::lexical_cast<int>(BreakTTLvector[2]);
  *(DNSResponse + numberReceived+8) = ThirdPartTTL;
  int FourthPartTTL = boost::lexical_cast<int>(BreakTTLvector[3]);
  *(DNSResponse + numberReceived+9) = FourthPartTTL;
  //Data Length of IP address
  *(DNSResponse + numberReceived+10) = 0;
  *(DNSResponse + numberReceived+11) = 4;
  /**Convert the string IP to integer and split into vector**/
  /**No need of using loop to convert string to int**/
  std::vector<std::string> BreakIPvector;
  boost::split(BreakIPvector, blPg, boost::is_any_of("."));
  int FirstPartIP = boost::lexical_cast<int>(BreakIPvector[0]);
  *(DNSResponse + numberReceived+12) = FirstPartIP;
  int SecondPartIP = boost::lexical_cast<int>(BreakIPvector[1]);
  *(DNSResponse + numberReceived+13) = SecondPartIP;
  int ThirdPartIP = boost::lexical_cast<int>(BreakIPvector[2]);
  *(DNSResponse + numberReceived+14) = ThirdPartIP;
  int FourthPartIP = boost::lexical_cast<int>(BreakIPvector[3]);
  *(DNSResponse + numberReceived+15) = FourthPartIP;
  
#ifdef DEBUG	
  printf("DNSResponse:\n");
  for(int i = 0; i < numberReceived+16; i++){
    printf( ":%02x", (unsigned char)DNSResponse[i] );	
  }
  std::cout << "Number of: " << numberReceived + 16 << std::endl;
#endif
  if(sendto(main_socket, DNSResponse, numberReceived + 16, 0, (struct sockaddr *)&local_info, socklen) == -1){
    std::cout << "Could not send the blocked request" << std::endl;
    setBlockedID(2);
    return 1;
  }
  return 0;
}//end creation of blocked response

