/**
 * @file
 * @author SNU <snu@csis.dk>
 * @author DCO <dco@csis.dk>
 * @brief Implementation of DNSNameserver
 *	
 * @section Description
 * This file contains the implementation of the DNSNameserver
 * singleton.
 * @TODO ensure that we throw more useful information 
 * @TODO Need to Read all the IPs from the DNS response. Now we are reading only the last IP 
 * @TODO Need to stop initializing global variables for threads
 * NONE of the buffer are being checked for out of bound limit
 */

#include"DNSNameserver.h"
#include"DNSstructure.h"
#include <vector>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/locks.hpp>
#include <map>
#include <iomanip>
#include <ios>
#include <iostream>
#include <DNSList.h>
#include <stdlib.h>
#include <cdb.h>
#include <boost/filesystem.hpp>



#define DNSLENGTH 512

void SendResponse(int main_socket, int socket_length, struct sockaddr_in server_in, int socket_leng, std::string req_buf, int req_size, std::string doma, int DFound, std::string resp);
MYSQL* createconn(std::string server, std::string user, std::string password, std::string database);
int writetolog( std::string completeDom, std::string domain, std::string client_ip, std::string response, std::string zoneName);


extern boost::unordered_map<std::string, std::string> whitelistMap;
extern boost::unordered_map<std::string, std::string> ChildlistMap;
extern boost::unordered_map<std::string, std::string> BlockedlistMap;
extern boost::unordered_map<std::string, std::string> malwarelistMap;
extern boost::unordered_map<std::string, std::string> RBLlistMap; 
extern boost::unordered_map<std::string, std::string> phishinglistMap;

extern std::multimap<std::string,std::string> MalList;
extern std::multimap<std::string,std::string> ChiList;

typedef struct SizeSocketPort{ 
  int DNSsize; 
  int Socketdes; 
  int PortNo;
  int MSocket;
  struct sockaddr_in servInfo;
}SSPM;

typedef struct StoreListString{
  std::string IPN;
  std::string strRBL;
  std::string Dom;
}SSTR;

int QueryMainNameserver(std::string QueryBuf,  
			std::string IPAddress, 
  			SSPM SiSoPoMain,
			struct sockaddr_in serverInfo, std::string* resp, std::vector<int> statusParser, int* dom, std::string doma);

void QueryRBLNameserverBack( SSPM SiSoPo, 
			     std::string MainQuery_buffer, 
			     struct sockaddr_in Mainlocal_info, 
			     SSTR StoreStr);

// Init static variable to NULL
Nameserver::DNSNameserver* Nameserver::DNSNameserver::mpsInstance = NULL;

// Constructor private (Singleton)
Nameserver::DNSNameserver::DNSNameserver()
{
}; //DNSNameserver

// Private
Nameserver::DNSNameserver::~DNSNameserver() 
{
}; //~DNSNameserver

// Static
Nameserver::DNSNameserver* Nameserver::DNSNameserver::GetInstance()
{
  if(mpsInstance == NULL) {
    mpsInstance = new Nameserver::DNSNameserver();
  }
  return mpsInstance;
}; //GetInstance


//Create the socket and listen to the UDP port
//@TODO Need to pass a structure for the unordered map
int Nameserver::DNSNameserver::StartNameServer(int local_port, 
						std::string LocalIPMain, 
						std::vector <int> statusParser){

  struct sockaddr_in local_info;
  socklen_t socklen	= sizeof( struct sockaddr_in );
  int Incoming_Socket = socket(AF_INET, SOCK_DGRAM, 0);
  struct hostent *mainServer;
  mainServer = gethostbyname(LocalIPMain.c_str());

  //creating a UDP socket
  if( -1 == Incoming_Socket ){	
    std::cout << "Could not create Socket" << std::endl;
    close(Incoming_Socket);
    throw 1;//Nameserver::MainException( "Primary Nameserver has a value zero");
  }//close creating socket

  //reset the sockaddress info 
  memset(&local_info, '\0', sizeof(struct sockaddr_in));
  local_info.sin_family = AF_INET;
  memmove((char *)&local_info.sin_addr, (char *)mainServer->h_addr, mainServer->h_length);
  local_info.sin_port = htons(local_port);
  
  //bind address to socket
  if(bind(Incoming_Socket, (struct sockaddr *)&local_info, socklen) == -1) {
    perror("Bind");
    close(Incoming_Socket);
    throw 1;// MainException("Could not Bind the Socket!!!: SNameSe()"); 
  }//close binding of socket
	int option_value = 1;
  if(0 != setsockopt(Incoming_Socket, SOL_SOCKET, SO_REUSEADDR, (void*)&option_value, sizeof(option_value))){
	std::cout <<  "Error setsocket" << std::endl;
	close(Incoming_Socket);
  }
  //call UDP Listen
 int ret = ListenUDP( Incoming_Socket, 
	     socklen, 
	     local_info,
	     statusParser);
 close(Incoming_Socket);
 return 1;
}//end creation of sockets with bind

//query the RBL NameServer
void QueryRBLNameserverBack( SSPM SiSoPo, 
			     std::string MainQuery_buffer, 
			     struct sockaddr_in Mainlocal_info, 
			     SSTR StoreStr
			     ){
  Nameserver::DNSstructure* mDNSstructure = Nameserver::DNSstructure::GetInstance();
  mDNSstructure->CreateQuery( SiSoPo.DNSsize, 
			      MainQuery_buffer, 
			      StoreStr.IPN,
			      SiSoPo.PortNo,
			      StoreStr.strRBL, 
			      StoreStr.Dom,
			      SiSoPo.MSocket, 
			      Mainlocal_info,
			      (unsigned)SiSoPo.Socketdes );

}//end querying RBL Nameserver


//find the domain name from the DNS query						    
std::string findName(int sizeQuery,  const char* CompleteQuery){

  int index = 0;
  char domain[DNSLENGTH];
  for(int i = 12; i < sizeQuery; i++) {
    int word_length = CompleteQuery[i];
    if(word_length <= 0) {				
      break;
    }
    else {
      int max = i + word_length;
      if(i != 12){
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

//split and Build URL
std::vector<std::string> SplitBuildURL(std::string domain, int BL){

  std::cout << "domain passed : " << domain << std::endl;
  std::vector<int> Delindex;	
  std::vector<std::string> URLlist;	

  std::string::size_type pos = domain.find_first_of(".");
  Delindex.push_back(pos+1);
  std::string Backdomain = domain;
  while(pos != std::string::npos){
    pos = Backdomain.find_first_of(".", pos + 1);
    Delindex.push_back(pos+1);
    std::cout << "Postion is: " << pos << std::endl;
  }
  URLlist.push_back(domain);
  for(int i = 0; i < (int)Delindex.size() - 2; i++){
    URLlist.push_back(domain.substr( Delindex[i], strlen(domain.c_str())));
    std::cout << "URLlist is: " << URLlist[i] << std::endl;
    if(0 == BL) break;
  }
  return URLlist;
}

std::string readDB( std::string dbfile, std::string key, int onlyValue ){
 	std::cout << "dbfile: " << dbfile << std::endl;
	std::cout << "key is: " << key << std::endl;
     int fd;
     std::string found = " ";
     // Open db file
     fd = open(dbfile.c_str(), O_RDONLY);
     if(fd == -1){
	std::cout << "ERROR" << std::endl;
	return " ";
     } else {
	//  cout << "OK" << endl;
     }
     std::string ValueString; ValueString.reserve(20);

	struct cdb cdb;
	char *val;
	unsigned vpos1, vlen1;

	cdb_init(&cdb, fd); // initialize internal structure 

  struct cdb_find cdbf; // structure to hold current find position 
  int err = cdb_findinit(&cdbf, &cdb, key.c_str(), key.size() ); // initialize search of key 
  if(err == 0) {
	perror("findinit No more key");
	cdb_free(&cdb);
	return found;
  }
  if(err < 0){
	perror("findinit Error");
	cdb_free(&cdb);
	return found;
  }
  
  while(cdb_findnext(&cdbf) > 0) {
    std::cout << "OOps found" << std::endl;
    if(onlyValue == 1) return "f";
    vpos1 = cdb_datapos(&cdb);
    vlen1 = cdb_datalen(&cdb);
    val =(char*)malloc(vlen1 * sizeof(char*));
    cdb_read(&cdb, val, vlen1, vpos1);
    std::string fo(val);
    found = fo;
    std::cout << val <<std::endl;
    // handle the value 
    free(val);
  }
  cdb_free(&cdb);
  // std::cout << "!val: " << val << std::endl;
  close(fd);
  return found;
}

bool DeleteFile(const std::string dirPath)
{
	const boost::filesystem::path thePath = boost::filesystem::system_complete(dirPath);
	try {
		remove_all(thePath);
	}
	catch (...) {
		std::cout << dirPath << "-file not deleted" << std::endl;
		return false;
	}
	return true;
}

int RecentFile( std::string f1, std::string f2 ){
	int seconds = time(NULL);			
	struct stat attrib1, attrib2;	
	if( stat(f1.c_str(), &attrib1) < 0 ){
		if ( stat(f2.c_str(), &attrib2)  == 0) return 2; //if cannot stat f1   
		return 0;
	}
	if ( stat(f2.c_str(), &attrib2)  < 0){
		if ( stat(f1.c_str(), &attrib2)  == 0) return 1; //if cannot stat f2
		return 0;	
	}
	int creation_time1 = attrib1.st_ctime;
	int creation_time2 = attrib2.st_ctime;
	if( creation_time1 > creation_time2 ){
		if( (seconds - creation_time1 ) > 60 ) 
			DeleteFile(f2);		
		return 1;
	}else{
		if( (seconds - creation_time2 ) > 60)
			DeleteFile(f1);
		return 2;
	}
	return 0;
}

std::string DetFile(std::string db1, std::string db2){
  std::string db;
  int recent;
  try{
  if( boost::filesystem::exists( db1 ) ){
	if( boost::filesystem::exists(db2) ){
		//look which one is the most recent
		recent = RecentFile(db1, db2);
		if( 0 == recent ) return " ";
		if ( 1 == recent ){
			std::cout << "Latest Creation: " << db1 << std::endl;
			db = db1;		
		}else{
			std::cout << "Latest Creation: " << db2 << std::endl;
			db = db2;
		}			
	}else{
			//Open whitelist.cdb
			db = db1;
	}	
  }else if( boost::filesystem::exists( db2 ) ){
	db = db2;
  }
  }catch(...){return " ";}
  std::cout << "db is: " << db << std::endl;
  return db;
}

//function to query all the databases
int QueryDatabases(std::string bl, std::string completeDomain, std::vector <int> statusParser, int main_s, socklen_t socket_l, struct sockaddr_in server_i){

    //Nameserver::DNSstructure* mDNSstructure = Nameserver::DNSstructure::GetpInstance();
    Nameserver::DNSList* mDNSList = Nameserver::DNSList::GetInstance();

    std::string clientIPAdd("Disabled");
    std::string comDom = completeDomain;
#ifdef DEBUG
	std::cout << "bl: " << bl  << std::endl;
	if(bl.compare(" ")) comDom = bl;
  	std::cout << "CompleteDomain: " << comDom << std::endl;
#endif 
  int BlockedID = mDNSList->GetBlockedID();
  if(statusParser[1] || statusParser[2] ) BlockedID = 2;
  int MALWARE_L = 1;
  int CHILD_L = 3;
  int Status = 100;
  int found = 0;
  if (statusParser[0]) {
   Status = MALWARE_L;
  }
  else if (statusParser[1]){
    Status = MALWARE_L;
  }
  else if(statusParser[2]){
    Status = CHILD_L;
  }
  /**Split the domain and Integrate**/
  std::vector<std::string> NewURLs = SplitBuildURL(completeDomain, 1);
  /****/
  std::vector<std::string> WWWremoved = SplitBuildURL(completeDomain, 0); 
  //determine the recent
  std::string white1 = "whitelist1.cdb"; std::string white2 = "whitelist2.cdb";
  std::string mal1 = "malware1.cdb"; std::string mal2 = "malware2.cdb";
  std::string chi1 = "child1.cdb"; std::string chi2 = "child2.cdb";

  std::string LatWhite = DetFile( white1, white2 );
  if(!LatWhite.compare(" ")) return 0;
  std::string LatMal = DetFile( mal1, mal2 );
  if(!LatMal.compare(" ")) return 0;
  std::string LatChi = DetFile( chi1, chi2 );
  if(!LatChi.compare(" ")) return 0;

  for(int i = 0; i < (int)WWWremoved.size(); i++){
	std::cout << "NewURLs: " << WWWremoved[i] << std::endl;
  }

  //lookup white list
  for(int x = 0; x < (int)NewURLs.size();x++){
    std::string r = readDB(LatWhite, NewURLs[x], 1);
    std::cout << "R is: " << r << std::endl;
    if( !(r.compare("f")) ){
	std::cout << "FOUND in Whitelist" << std::endl;
	return 2;
    }else{ std::cout << "Not Found" << std::endl;}
  }
  boost::thread_group thLog;  std::string unifiedResp;
  switch( Status ){
  case 0://lookup RBL //Not operational because the RBL database is not yet designed
 
    break;
  case 1://lookup Malware
	std::cout << "Looking the malware database: "  << (int)WWWremoved.size() << std::endl;
	std::cout << "completeDomain: " << comDom << std::endl;
    	for(int j = 0; j < (int)NewURLs.size(); j++){
		unifiedResp = readDB(LatMal, NewURLs[j], 0);
		if(unifiedResp.compare(" ")){
			std::cout << "unifiedResponse: " << unifiedResp << std::endl;
			thLog.add_thread( new boost::thread(writetolog, comDom, NewURLs[j], 
					clientIPAdd, unifiedResp, 
					mDNSList->GetZoneName() )); 
			found = 1;
		}//end if statement
	}//end for loop
	if (found == 1) return 3;
      case 3://lookup Child
 	std::cout << "Looking the Child database: "  << (int)WWWremoved.size() << std::endl;
	for(int j = 0; j < (int)NewURLs.size(); j++){
		unifiedResp = readDB(LatChi, NewURLs[j], 0);
		if( unifiedResp.compare(" ") ){
			thLog.add_thread( new boost::thread(writetolog, comDom, NewURLs[j], 
					clientIPAdd, unifiedResp, 
					mDNSList->GetZoneName() )); 
			found = 1;
		}//end if statement
	}//end for loop
	if (found == 1) return 3;
    break;
  default:
    std::cout << "No database Parsed. WHY??. This should not happen" << std::endl;
    return 0;
    break;
  }
#ifdef PRODUCTION
  std::cout << "Domain not found within Database" << std::endl;
#endif
  return 0;	
}//end query databases

//not using this function to create the connection
MYSQL* createconn(std::string server, std::string user, std::string password, std::string database)
{
  MYSQL* conn = mysql_init(NULL);
#ifdef DEBUG
	std::cout <<  "server: "  << server << "e" <<std::endl;
	std::cout <<  "user: "	<< user << "e" << std::endl;
	std::cout << "pass: " << password << "e" << std::endl;
	std::cout << "data: " << database << "e" << std::endl;
#endif
  /* Connect to database */
  if (!mysql_real_connect(conn, server.c_str(), user.c_str(), password.c_str(), database.c_str(), 0, NULL, 0)) {
    fprintf(stderr, "%s\n", mysql_error(conn));
     mysql_close(conn);
  }
  return conn;
}//end create connection


//function to log the blocked IP and domains to database
int writetolog( std::string compleDom,std::string domain, std::string client_ip, std::string response, std::string zoneName)   
{
  Nameserver::DNSList* mDNSList = Nameserver::DNSList::GetInstance();

#ifdef DEBUG
  std::cout << "ZN in writelog: " << zoneName << std::endl;
  std::cout << "client_ip: " << client_ip << std::endl;
  std::cout << "compleDom" << compleDom << std::endl;
  std::cout << "GetDatabaseIP" << mDNSList->GetDatabaseIP() << "e" <<std::endl;
  std::cout << "GetDatabaseUser" << mDNSList->GetDatabseUser() << "e" <<std::endl;
  std::cout << "GetDatabasePass" << mDNSList->GetDatabasePass() << "e" <<std::endl;
  std::cout << "GetDatabaseName" << mDNSList->GetDatabaseName() << "e" << std::endl;
#endif
  MYSQL* connectionDat = mysql_init(NULL);
  /* Connect to database */
  if (!mysql_real_connect(connectionDat, mDNSList->GetDatabaseIP().c_str(), mDNSList->GetDatabseUser().c_str(), mDNSList->GetDatabasePass().c_str(), mDNSList->GetDatabaseName().c_str(), 0, NULL, 0)) {
    fprintf(stderr, "%s\n", mysql_error(connectionDat));
     mysql_close(connectionDat);
     return 0;
  }
  char query[DNSLENGTH];
  memset(&query, 0, DNSLENGTH);
  int query_length = sizeof("INSERT INTO blockeddomainstats (clientip, domain, blockeddomain, response, zone_name) values('");
	
  strncat(query,"INSERT INTO blockeddomainstats (clientip, domain, blockeddomain, response, zone_name) values('", query_length);
  strncat(query, client_ip.c_str(), strlen(client_ip.c_str()) );

  //log the blocked domain
  strncat(query, "','", 3);
  strncat(query, domain.c_str(), strlen(domain.c_str()));

  //log the complete domain
  strncat(query, "','", 3);
  strncat(query, compleDom.c_str(), strlen(compleDom.c_str()));

  strncat(query, "','", 3);
  strncat(query, response.c_str(), strlen(response.c_str()));
  
  strncat(query, "','", 3);
  //add the zoneName
  strncat(query, zoneName.c_str(), strlen(zoneName.c_str()));
  
  strncat(query, "')", 2);
#ifdef DEBUG	
  std::cout << "query: " << query << std::endl;
#endif
  // send SQL query
  if (mysql_query(connectionDat, query)) {
    fprintf(stderr, "%s\n", mysql_error(connectionDat));
    std::cout << "Could Not input to the database the log info: From writetolog" << std::endl;
    return 1;
  }
  mysql_close(connectionDat);
  return 0;
}//end writetolog


int ThreadIncomingTraffic(int DNSQuerySize, 
			   int main_socket,
			   std::string Query_buffer, 
			   struct sockaddr_in server_info,  
			   socklen_t socket_length, 
			   std::vector <int> statusParser){
  Nameserver::DNSList* mDNSList = Nameserver::DNSList::GetInstance();

  int PortNameserver =  mDNSList->GetPortNameserver();
  std::string IPNameserver = mDNSList->GetIPNameserver();

  //retrieve the name from the query
  std::string domain = findName(DNSQuerySize, Query_buffer.c_str());

  std::string IPRBLResponse;
  std::string ZoneName;
  std::string ListSuffix[3] = {".rhsbl.ahbl.org", ".dnsbl.sorbs.net", ".multi.surbl.org" /*, ".dbl.spamhaus.org", ".hostkarma.junkemailfilter.com"*/ };
  struct sockaddr_in BackMainServer = server_info;
  std::string BackIPName = IPNameserver;
 
  SSPM SiSoPo;
  SiSoPo.DNSsize = DNSQuerySize;
  SiSoPo.Socketdes = socket_length;
  SiSoPo.PortNo = PortNameserver;
  SiSoPo.MSocket = main_socket;
  SiSoPo.servInfo = server_info;
  SSTR StoreString;
  /**End Test**/
  int domainFound = 0;
  std::string resp;

  mDNSList->setBlockedID(2);

  //lookup domain in databases
  domainFound = QueryDatabases(" ",domain, statusParser, main_socket, socket_length, server_info ); 
#ifdef DEBUG
  std::cout << "domainFound" << domainFound << std::endl;
  std::cout << "DOMAIN IS: "  << domain << std::endl;
  std::cout << "DETERMINE THE SIZE OF THE LISTSUFFIX: " << (int)sizeof(ListSuffix)/(int)sizeof(ListSuffix[0]) << std::endl;
  std::cout << "ListSuffix[0]"  << ListSuffix[0] << std::endl;
  std::cout << "ListSuffix[1]"  << ListSuffix[1] << std::endl;
#endif
  boost::thread_group thgp, thMain;
 
  if(domainFound == 0 || domainFound == 2){
   std::string IPadd = mDNSList->GetIPNameserver();
#ifdef DEBUG
   std::cout << "IPadd: " << IPadd << std::endl;
#endif
   thMain.add_thread(new boost::thread(QueryMainNameserver, Query_buffer, IPadd, SiSoPo, BackMainServer, &resp, statusParser, &domainFound, domain));
  }
   
  //we only care if the RBLStatus is true
  if(/*domainFound != 3 &&*/ domainFound != 2){
    if (statusParser[0]){
#ifdef DEBUG
      std::cout << "RBL Lookup" << std::endl;
#endif
      if(DNSQuerySize > 0){	
	//function to monitor the other threads for one value
	//create the different threads
	for(int currentThread = 0; currentThread < 3; currentThread++){
	  std::string RBLDomain = domain;
	  RBLDomain += ListSuffix[currentThread];
#ifdef DEBUG
	  std::cout << "domainName: " << RBLDomain <<  std::endl;
#endif
	  std::string strRBLDomain(RBLDomain);	
	  StoreString.IPN = IPNameserver;
 	  StoreString.strRBL = strRBLDomain;
	  StoreString.Dom = domain;   
	
/*	  thgp.add_thread( new boost::thread( QueryRBLNameserverBack, 
					      SiSoPo,
					      Query_buffer,
					      server_info,  
					      StoreString ));
*/	}// end creation of threads
      }
    }
  }
  thMain.join_all();
#ifdef DEBUG
  std::cout << "before thread: domainFound is: " << domainFound <<  std::endl;
#endif  
  SendResponse( main_socket, socket_length, server_info, socket_length, Query_buffer, DNSQuerySize, domain, domainFound, resp);
#ifdef DEBUG
  std::cout << "BLOCKEDID DETECTED: "  << mDNSList->GetBlockedID() << std::endl;
#endif   
 return 0;
}//end Thread

//Read the IP from the Response
std::string DetIPfromResponse( unsigned char* resp_buf, int bytecnt){

  //determine the IP from the response
  char ip[12];
  char completeIP[12];
  if(bytecnt < 5) return "";
  sprintf(ip, "%i", resp_buf[bytecnt-4]);
  strcpy(completeIP,ip);
  strncat(completeIP, ".", 1);
  for(int i = bytecnt - 3; i < bytecnt; i++){
    sprintf(ip, "%i", resp_buf[i]);
    strcat(completeIP, ip );
    if( i != bytecnt - 1)
      strncat(completeIP, ".", 1);
  }
  std::string IPResponse(completeIP);
#ifdef DEBUG
  std::cout << "Last IP is : " <<  completeIP << std::endl;
#endif
  return IPResponse;
}
//int NS;
int QueryMainNameserver( std::string request_buffer, 
			 std::string IP_Address,
			 SSPM SiSoPoMain,
			 struct sockaddr_in server_info, std::string* resp, std::vector<int> status, int* dom, std::string doma){

  int nameserver_socket;  unsigned char response_buf[DNSLENGTH];
  int request_size = SiSoPoMain.DNSsize;
  int remote_port = SiSoPoMain.PortNo;

  struct sockaddr_in destination_info, nameserver_info; 
  socklen_t socklen = sizeof(struct sockaddr_in);  	
  struct hostent *primary_nameserver;
  primary_nameserver = gethostbyname(IP_Address.c_str());

/**TEST**/
//struct hostent *secondary_nameserver = gethostbyname("192.168.231.23");
//if (secondary_nameserver == 0) { 
//   perror("Invalid secondary nameserver provided");
//    return 1;
//}
//struct sockaddr_in sec_destination_info;

/****/

  if (primary_nameserver == 0) { 
    perror("Invalid primary nameserver provided");
    return 1;
  }
#ifdef DEBUG 
 std::cout << "Nameserver" << std::endl;
#endif
  nameserver_socket = socket(AF_INET, SOCK_DGRAM, 0);	
  if(nameserver_socket < 0) {
    perror("Creation of UDP socket failed for Main");
    close(nameserver_socket); 
    return 1;
  }
  Nameserver::DNSList* mDNSList = Nameserver::DNSList::GetInstance();
  //if( NS != 1 && NS != 2 )  NS = 1;
  destination_info.sin_family = AF_INET;
#ifdef DEBUG
   //std::cout << "NS(): " << NS << std::endl;
#endif
 // if( 1 == NS )
  	memmove((char *)&destination_info.sin_addr, (char *)primary_nameserver->h_addr, primary_nameserver->h_length);
//  else
//	memmove((char *)&destination_info.sin_addr, (char *)secondary_nameserver->h_addr, secondary_nameserver->h_length);
  destination_info.sin_port = htons(remote_port);
#ifdef DEBUG
  std::cout << "sendto: " << strlen(request_buffer.c_str()) << std::endl;
  const char* h = request_buffer.c_str();	
  for(int i = 0; i < 600; i++){
	printf(":%c", h[i]);	
  } 
  printf("\n");
  for(int i = 0; i < 600; i++){
	printf(":%02x", h[i]);	
  }
  std::cout << "Request_size: " << request_size << std::endl;
#endif

  if (sendto(nameserver_socket, request_buffer.c_str(), request_size, 0, (const struct sockaddr *)&destination_info, socklen) == -1) {
    perror("Could not perform DNS request ");
    close(nameserver_socket);   
    return 1;
  }
  memset(response_buf, '\0', sizeof(response_buf));
  int bytecount = 0;
  struct timeval timeout;
  fd_set fds1;
  FD_ZERO(&fds1);
  FD_SET(nameserver_socket, &fds1);
/****/
  int x;
  x=fcntl(nameserver_socket, F_GETFL,0);
  fcntl(nameserver_socket, F_SETFL,x | O_NONBLOCK);
/****/
  timeout.tv_sec = 1;
  timeout.tv_usec = 500000;
  int rc = select(nameserver_socket + 1, &fds1, NULL, NULL, &timeout);
  if( -1 == rc){
    std::cout << "Could not send the blocked request" << std::endl;
    close(nameserver_socket);
    return 1;
  }
  if (rc > 0){
    if(FD_ISSET(nameserver_socket, &fds1)){
 	if((bytecount = recvfrom(nameserver_socket, response_buf, sizeof(response_buf), 0, (struct sockaddr *)&nameserver_info, &socklen)) == -1){
  		perror("Error Receiving: \n"); 
		close(nameserver_socket); 
		return 1;	
	}
    } 
}
#ifdef DEBUG
  std::cout <<  "bytecount: " << bytecount << std::endl;
  std::cout << "Passed rcvfrom" << std::endl;
#endif

 if(bytecount > DNSLENGTH || bytecount < 1) {
    std::cout << "The byte count is greater or less than 512: " << bytecount << std::endl;
    close(nameserver_socket); 
    //mDNSList->setResStr(" ");
  //  if(NS =l= 1){
    //	NS=2;
	//std::cout << "NEW NS is:"<< NS  << std::endl;
//}
 //   else
//	NS=1;
    return 1;
  }
  //Nameserver::DNSstructure* mDNSstructure;
  //mDNSstructure = new Nameserver::DNSstructure;
  Nameserver::DNSstructure* mDNSstructure = Nameserver::DNSstructure::GetInstance();
 

 //std::string IPfromResponse = DetIPfromResponse(response_buf, bytecount);
 //if IPfromResponse empty

  std::string RealHostname = " ";
  std::vector<std::string> IPlists;
  mDNSstructure->printresponsedata(request_buffer.c_str(), response_buf, bytecount, IPlists, &RealHostname );

  std::cout << "realhost lookup: " << RealHostname << std::endl;
  //lookup domain in databases
  if(RealHostname.compare(" "))
     *dom = QueryDatabases(doma, RealHostname, status, SiSoPoMain.MSocket, SiSoPoMain.Socketdes, SiSoPoMain.servInfo ); 
  std::cout << "dom is: " << *dom << std::endl;
  std::cout << "RL: "  << RealHostname << "size ip: " << IPlists.size() << std::endl;

  if(*dom != 3 ){
	for(int i =0 ;i < (int)IPlists.size(); i++){
		std::cout << "ip is : " << IPlists[i]  << std::endl;
		boost::unordered_map<std::string, std::string>::iterator iteratorBlockedIP;
		//look in the database if the IP is blocked
		iteratorBlockedIP = BlockedlistMap.find(IPlists[i]);
		if (iteratorBlockedIP != BlockedlistMap.end()){
			std::cout << "Found the domain in the BLOCKED IP list" << std::endl;
			*dom = 3;
		}
	}
  }
  std::string ResStr;
  for (int i = 0; i < bytecount; i++){
      ResStr.push_back(response_buf[i]);
  }
  mDNSList->setByteCnt(bytecount);
  *resp = ResStr;
#ifdef DEBUG
  std::cout << "Before close: " << ResStr << std::endl;
#endif
  close(nameserver_socket); 
  //delete mDNSList;
  return 0; 
}

void SendResponse(int main_socket, int socket_length, struct sockaddr_in server_in, int socket_leng, std::string req_buf, int req_size, std::string doma, 
int domFound, std::string resp){
#ifdef DEBUG
  std::cout << "BlockedID is: " << domFound << std::endl;
#endif
 Nameserver::DNSList* mDNSList = Nameserver::DNSList::GetInstance();

  if( 1 == domFound ){
#ifdef DEBUGp
    std::cout << "Found in RBL: " << mDNSList->GetBlockedID() << std::endl;
#endif
  }else if (domFound == 2 || domFound == 0){	
#ifdef DEBUG
    std::cout << "A normal Request: " << resp << std::endl;
#endif
    if(sendto(main_socket, resp.c_str(), 512/*mDNSList->GetByteCnt()*/, 0, (struct sockaddr *)&server_in, socket_leng)==-1){
	perror("send error!");
}
	//mDNSList->setResStr(" "); 
  }else if (3 == domFound){
#ifdef DEBUG
    std::cout << "Found in Database: " << mDNSList->GetBlockedID() << std::endl;
#endif 
    Nameserver::DNSstructure* mDNSstructure = Nameserver::DNSstructure::GetInstance();
    mDNSstructure->CreateBlockedResponse(req_buf, 
					 req_size, 
					 main_socket, server_in, 
					 socket_length,"No DOmain");
  }
}//end SendResponse


//listen to the UDP pallckets from the specified port
int Nameserver::DNSNameserver::ListenUDP(int main_socket, 
					  socklen_t socket_length, 
					  struct sockaddr_in server_info, 
					  std::vector <int> statusParser){  

  //int count = 0;
  //loop forever listening for traffic
  while(1){
    int DNSQuerySize = 0;
    boost::thread_group th;
    int BUPmain_socket = main_socket;
    socklen_t BUPsocket_length = socket_length;
    struct sockaddr_in BUPserver_info = server_info;  
    unsigned char Query_buffer[DNSLENGTH];
    memset(&Query_buffer, '\0', DNSLENGTH);
    std::string queryStr = "";
#ifdef DEBUG
    std::cout << "Waiting..." << std::endl;
#endif
    //receive the DNS requesnt
    DNSQuerySize = recvfrom(BUPmain_socket, Query_buffer, DNSLENGTH, 0, (struct sockaddr *)&BUPserver_info, &BUPsocket_length);  
    if(DNSQuerySize != -1){  
	for (int i = 0; i < DNSQuerySize; i++){
		queryStr.push_back(Query_buffer[i]);
	}
	#ifdef DEBUG
	std::cout <<  "Length query Char: " << DNSQuerySize  << std::endl;
	for (int i = 0; i < DNSQuerySize;i++ ){
		printf("%02x", Query_buffer[i]);
	}
	#endif
	    th.add_thread( new boost::thread( ThreadIncomingTraffic, DNSQuerySize, BUPmain_socket, queryStr, BUPserver_info, BUPsocket_length, statusParser));
    }
  }//end while loop
return 1;
}//end Listen UDP packets

