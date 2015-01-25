/**
 * @file
 * @author DCO <dco@csis.dk>
 * @author SNU <snu@csis.dk>
 * @brief Implementation of DNSNameserver
 */

#include <iostream>
#include <DNSNameserver.h>
#include <DNSstructure.h>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <mysql/mysql.h>
#include <yaml.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>

#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>
#include <sys/types.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <DNSList.h>

using namespace std;
using namespace Nameserver;

std::string IpName;
int PortName = 0;
std::string blPg = "";
std::string ttl = "";
std::string PathCDB = "";

//function to parse the config file
int ParseConfig(std::string ConfigPath, std::vector<std::string> & SplitPorts, 
		 std::vector<std::string> & SplitMainIP, 
		 std::vector<std::string> & SplitNameserver, 
		 std::vector<std::string> & SplitDatabaseInfo, 
		 std::vector<std::string> & SplitBlockedPg,
		 std::vector<std::string> & SplitTTL){

  FILE *FileHandler = fopen(ConfigPath.c_str(), "r");
 
  yaml_parser_t Yamlparser; 
  yaml_event_t Yamlevent, YE;

  if(!yaml_parser_initialize(&Yamlparser)){
    return 1;// MainException("Yaml parser Initialization Failure");
  }
  if( NULL == FileHandler){
    return 1;//MainException( "Config File Handler Exception");
  }
  yaml_parser_set_input_file(&Yamlparser, FileHandler);
  std::string PortNums, NameserverInfo, DatabaseInfo, MainIP, BlockedPgInfo, sttl;

  do{
    //Call YAML parser
    yaml_parser_parse(&Yamlparser, &Yamlevent);
    if(Yamlevent.type == YAML_SCALAR_EVENT){
      std::string actual((char*)Yamlevent.data.scalar.value);
      //determine the list of ports
      std::string Portlist = "ListenPort";
      if( strcmp( actual.c_str(), Portlist.c_str() ) ) {
      }else{
	yaml_parser_parse(&Yamlparser, &YE);
	PortNums = (char*)YE.data.scalar.value;
      }
	//determine the MainServer IP address
      std::string MainServerIP = "MainServer";
      if( strcmp( actual.c_str(), MainServerIP.c_str() ) ) {
      }else{
	yaml_parser_parse(&Yamlparser, &YE);
	MainIP = (char*)YE.data.scalar.value;
      }
      //determine the Nameserver Info
      std::string Nameserver = "Nameserver";
      if( strcmp( actual.c_str(), Nameserver.c_str() ) ) {
      }else{
	yaml_parser_parse(&Yamlparser, &YE);
	NameserverInfo = (char*)YE.data.scalar.value; 
      }
      //determine the Info for the Database
      std::string Database = "Database";
      if( strcmp( actual.c_str(), Database.c_str() ) ) {
      }else{
	yaml_parser_parse(&Yamlparser, &YE);
	DatabaseInfo = (char*)YE.data.scalar.value; 
      }

	//determine the IP of Blocked Page
      std::string blockedPg = "BlockedPage";
      if( strcmp( actual.c_str(), blockedPg.c_str() ) ) {
      }else{
	yaml_parser_parse(&Yamlparser, &YE);
	BlockedPgInfo = (char*)YE.data.scalar.value; 
      }

	//determine the Time to Live
      std::string ttl1 = "TTL";
      if( strcmp( actual.c_str(), ttl1.c_str() ) ) {
      }else{
	yaml_parser_parse(&Yamlparser, &YE);
	sttl = (char*)YE.data.scalar.value; 
      }

      //determine Path to CDB files
      std::string PCDB = "PathCDB";
      if( strcmp( actual.c_str(), PCDB.c_str() ) ) {
      }else{
	yaml_parser_parse(&Yamlparser, &YE);
	PathCDB = (char*)YE.data.scalar.value;
      }
	std::cout << "PCDB: " << PathCDB << std::endl;
	if(YE.type != YAML_STREAM_END_EVENT){
		yaml_event_delete(&Yamlevent);
		yaml_event_delete(&YE);	
	}
	
    }
	//std::cout << "Within While loop"  << std::endl;    
  }while(Yamlevent.type != YAML_STREAM_END_EVENT);//end while lop
  std::cout << "Ports: "  << PortNums << std::endl;
  std::cout << "MainIP: " << MainIP <<std::endl;
  std::cout << "NameserverInfo: " << NameserverInfo << "l" << std::endl;
  std::cout << "DatabaseInfo: " << DatabaseInfo << std::endl;
  //split the different strings
  boost::split(SplitPorts, PortNums, boost::is_any_of(" "));
  SplitMainIP.push_back(MainIP);
  boost::split(SplitNameserver, NameserverInfo, boost::is_any_of(" "));
  boost::split(SplitDatabaseInfo, DatabaseInfo, boost::is_any_of(" "));
  SplitBlockedPg.push_back(BlockedPgInfo);
  SplitTTL.push_back(sttl);
  //boost::split(SplitTTL, sttl, boost::is_any_of(" "));
  //End the event, parser and close the file
  yaml_event_delete(&Yamlevent);  
  yaml_parser_delete(&Yamlparser);
  fclose(FileHandler);
  return 1;

}//end parseConfig



/**Main Function**/
/*Steps: 1/ Parse the Config File */
/*     : 2/ Load to Memory the Databases */
/*     : 3/ Start the Nameserver 
**/
int main(int argc, char* argv[]){

  if(argc > 2 || argc < 2){
    std::cout << "Config Path missing: E.g. <path to MainDNSserver> config.yaml" << std::endl;
    return 1;
  }   
  std::string PathConfig(argv[1]);
  
  std::cout << "Parameter is: " << PathConfig << std::endl;

  /***Retrieve Info from the Config file**/
  std::vector<std::string> SplitPt;
  std::vector<std::string> SplitMain;
  std::vector<std::string> SplitNS;
  std::vector<std::string> SplitDBInfo;
  std::vector<std::string> SplitBlocked;
  std::vector<std::string> SplitTTL;

  int ret = ParseConfig(PathConfig, SplitPt, SplitMain, SplitNS, SplitDBInfo, SplitBlocked, SplitTTL);

/* 
  std::string NameserverIP = "8.8.8.8";
  std::string NameserverPort = "53";
 
  std::string DBIP = "192.168.230.56";
  std::string DBName = "screenDNS";
  std::string DBPwd = "screenDNS#11";
  std::string DBUser = "admin";
*/

  //List the port in an array of integers
  std::vector<int> PortArray;	
  for(unsigned int it = 0; it < SplitPt.size(); ++it) {
    std::cout << "Port: " << SplitPt[it] << std::endl;
    PortArray.push_back(boost::lexical_cast<int>(SplitPt[it]));
  }
  //Retrieve the IP of the server the program is running on
  std::string MainServerIPAddress = SplitMain[0].substr(3, strlen(SplitMain[0].c_str()) - 3 ); 
  std::cout << "SplitMain[0]: " << SplitMain[0] << std::endl; std::cout << "SplitNS[0]: " << SplitNS[0] << std::endl;
  //Retrieve the IP and Port number of the external nameserver 
  std::string NameserverIP = SplitNS[0].substr(3, strlen(SplitNS[0].c_str()) - 3 );
  std::string NameserverPort = SplitNS[1].substr(5, strlen(SplitNS[1].c_str()) - 5 );
  int PortNameServer = boost::lexical_cast<int>(NameserverPort);
  std::cout << "NameserverIP: " << NameserverIP << "l" <<std::endl;
  std::cout << "NameserverPort: " << PortNameServer << "l" <<std::endl;

  blPg = SplitBlocked[0].substr(3, strlen(SplitBlocked[0].c_str()) - 3 );
  std::cout << "Blocked IP: " << "l" << blPg << std::endl;

  //create an instance of DNSstructure
 Nameserver::DNSList* mDNSList = Nameserver::DNSList::GetInstance();

 //mDNSstructure->setIPNameserver(NameserverIP);
 mDNSList->setIPNameserver(NameserverIP);
 mDNSList->setPortNameserver(PortNameServer);
 IpName = NameserverIP;
 PortName = PortNameServer;

  //Retrieve all the necessary info. to establish the connection to the database
  std::string DBIP = SplitDBInfo[0].substr(3, strlen(SplitDBInfo[0].c_str()) - 3);
  std::string DBName = SplitDBInfo[1].substr(5, strlen(SplitDBInfo[1].c_str()) - 5);
  std::string DBPwd = SplitDBInfo[2].substr(9, strlen(SplitDBInfo[2].c_str()) - 9);
  std::string DBUser = SplitDBInfo[3].substr(5, strlen(SplitDBInfo[3].c_str()) - 5 );

//Nameserver::DNSList* mDNSList = Nameserver::DNSList::GetInstance();
  
  ttl = SplitTTL[0].substr(5, strlen(SplitTTL[0].c_str()) - 5 );
  std::cout << "ttl: " << ttl << std::endl;
  std::cout << "DBIP: " << DBIP << std::endl;
  std::cout << "DBName: " << DBName << std::endl;
  std::cout << "DBPwd: " << DBPwd << std::endl;
  std::cout << "DBUser: " << DBUser << std::endl;
  delete mDNSList;

#ifdef DEBUG
  std::cout << "mDNSList->getDatabaseIP: " <<  mDNSList->GetDatabaseIP() << std::endl;
  std::cout << "mDNSList->getDatabseUser: " << mDNSList->GetDatabseUser() << std::endl;
  std::cout << "mDNSList->getDatabasePass: " <<  mDNSList->GetDatabasePass() << std::endl;
  std::cout << "mDNSList->getDatabaseName: " <<  mDNSList->GetDatabaseName() << std::endl;
#endif

 //create three processes
  pid_t  pid;
  pid = fork();
 if (0 == pid){ //child process
      std::vector <int> statusParser;
      int RBL = 1;
      statusParser.push_back(RBL);
      statusParser.push_back(0); statusParser.push_back(0);
      Nameserver::DNSNameserver* mDNSServer53;
      mDNSServer53 = new Nameserver::DNSNameserver;
      //call the function to start the nameserver
      int ret2 = mDNSServer53->StartNameServer( PortArray[0], MainServerIPAddress, statusParser);
	std::cout << "After ret" << std::endl;
	delete mDNSServer53;
	return ret2;
  }	  
  else {//parent process
    pid_t  pidPar;
    pidPar = fork();
    if (pidPar == 0){
      std::vector <int> statusParser;
      int MALWARE = 1;
      Nameserver::DNSNameserver* mDNSServer20053;
      mDNSServer20053 = new Nameserver::DNSNameserver;
      statusParser.push_back(0);statusParser.push_back(MALWARE);statusParser.push_back(0);
      //RBL =false; MALWARE =true; CHILD = false;
	//call the function to start the nameserver
	int ret3 = mDNSServer20053->StartNameServer( PortArray[1], MainServerIPAddress, statusParser);
	delete mDNSServer20053;
	return ret3;
    }		  
    else{
      std::vector <int> statusParser;
      int CHILD;
      Nameserver::DNSNameserver* mDNSServer30053;
      mDNSServer30053 = new Nameserver::DNSNameserver;
      statusParser.push_back(0);statusParser.push_back(0);statusParser.push_back(CHILD);
      //RBL = false; MALWARE = false; CHILD = true;
	//call the function to start the nameserver
	int ret4 = mDNSServer30053->StartNameServer( PortArray[2], MainServerIPAddress, statusParser );
	delete mDNSServer30053;
	return ret4;
    }		  
  }
  return 1;
}//end main
