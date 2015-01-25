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
#include <boost/unordered_map.hpp>
#include <yaml.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
//#include "ExceptionHandler.h"
//#include <ServerException.h>

#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>
#include <sys/types.h>
//#include <linux/unistd.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <DNSList.h>

using namespace std;
using namespace Nameserver;



boost::unordered_map<std::string, std::string> whitelistMap;
boost::unordered_map<std::string, std::string> RBLlistMap; 
boost::unordered_map<std::string, std::string> BlockedlistMap;





//function to parse the config file
int ParseConfig(std::string ConfigPath, std::vector<std::string> & SplitPorts, 
		 std::vector<std::string> & SplitMainIP, 
		 std::vector<std::string> & SplitNameserver, 
		 std::vector<std::string> & SplitDatabaseInfo){

  FILE *FileHandler = fopen(ConfigPath.c_str(), "r");
  yaml_parser_t Yamlparser; 
  yaml_event_t Yamlevent;

  if(!yaml_parser_initialize(&Yamlparser)){
    throw 1;// MainException("Yaml parser Initialization Failure");
  }
  if( NULL == FileHandler){
    throw 1;//MainException( "Config File Handler Exception");
  }
  yaml_parser_set_input_file(&Yamlparser, FileHandler);
  std::string PortNums, NameserverInfo, DatabaseInfo, MainIP;
  do {
    //Call YAML parser
    yaml_parser_parse(&Yamlparser, &Yamlevent);
    if(Yamlevent.type == YAML_SCALAR_EVENT){
      std::string actual((char*)Yamlevent.data.scalar.value);
      //determine the list of ports
      std::string Portlist = "ListenPort";
      if( strcmp( actual.c_str(), Portlist.c_str() ) ) {
      }else{
	if(Yamlevent.type != YAML_STREAM_END_EVENT)
	  yaml_event_delete(&Yamlevent);	
	yaml_parser_parse(&Yamlparser, &Yamlevent);
	PortNums = (char*)Yamlevent.data.scalar.value; 
      }
      //determine the MainServer IP address
      std::string MainServerIP = "MainServer";
      if( strcmp( actual.c_str(), MainServerIP.c_str() ) ) {
      }else{
	if(Yamlevent.type != YAML_STREAM_END_EVENT)
	  yaml_event_delete(&Yamlevent);
	yaml_parser_parse(&Yamlparser, &Yamlevent);
	MainIP = (char*)Yamlevent.data.scalar.value;
      }
      //determine the Nameserver Info
      std::string Nameserver = "Nameserver";
      if( strcmp( actual.c_str(), Nameserver.c_str() ) ) {
      }else{
	if(Yamlevent.type != YAML_STREAM_END_EVENT)
	  yaml_event_delete(&Yamlevent);	
	yaml_parser_parse(&Yamlparser, &Yamlevent);
	NameserverInfo = (char*)Yamlevent.data.scalar.value; 
      }
      //determine the Info for the Database
      std::string Database = "Database";
      if( strcmp( actual.c_str(), Database.c_str() ) ) {
      }else{
	if(Yamlevent.type != YAML_STREAM_END_EVENT)
	  yaml_event_delete(&Yamlevent);	
	yaml_parser_parse(&Yamlparser, &Yamlevent);
	DatabaseInfo = (char*)Yamlevent.data.scalar.value; 
      }	
    }
  } while(Yamlevent.type != YAML_STREAM_END_EVENT);

  std::cout << "Ports: "  << PortNums << std::endl;
  std::cout << "NameserverInfo: " << NameserverInfo << std::endl;
  std::cout << "DatabaseInfo: " << DatabaseInfo << std::endl;
  //split the different strings
  boost::split(SplitPorts, PortNums, boost::is_any_of(" "));
  boost::split(SplitMainIP, MainIP, boost::is_any_of(" "));
  boost::split(SplitNameserver, NameserverInfo, boost::is_any_of(" "));
  boost::split(SplitDatabaseInfo, DatabaseInfo, boost::is_any_of(" "));
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
  
  int ret = ParseConfig(PathConfig, SplitPt, SplitMain, SplitNS, SplitDBInfo);
  
  /*List the port in an array of integers*/
  std::vector<int> PortArray;	
  for(unsigned int it = 0; it < SplitPt.size(); ++it) {
    std::cout << "Port: " << SplitPt[it] << std::endl;
    PortArray.push_back(boost::lexical_cast<int>(SplitPt[it]));
  }
  /*Retrieve the IP of the server the program is running on*/
  std::string MainServerIPAddress = SplitMain[0].substr(3, strlen(SplitMain[0].c_str()) - 3 );
 
  /*Retrieve the IP and Port number of the external nameserver */
  std::string NameserverIP = SplitNS[0].substr(3, strlen(SplitNS[0].c_str()) - 3 );
  std::string NameserverPort = SplitNS[1].substr(5, strlen(SplitNS[1].c_str()) - 5 );
  int PortNameServer = boost::lexical_cast<int>(NameserverPort);
  std::cout << "NameserverIP: " << NameserverIP << std::endl;
  std::cout << "NameserverPort: " << PortNameServer << std::endl;

  //create an instance of DNSstructure
 Nameserver::DNSList* mDNSList = Nameserver::DNSList::GetInstance();

 //mDNSstructure->setIPNameserver(NameserverIP); 
 mDNSList->setIPNameserver(NameserverIP);
 mDNSList->setPortNameserver(PortNameServer);
  
 Nameserver::DNSstructure* mDNSstructure = Nameserver::DNSstructure::GetInstance();
 
  /*Retrieve all the necessary info. to establish the connection to the database*/
  std::string DBIP = SplitDBInfo[0].substr(3, strlen(SplitDBInfo[0].c_str()) - 3);
  std::string DBName = SplitDBInfo[1].substr(5, strlen(SplitDBInfo[1].c_str()) - 5);
  std::string DBPwd = SplitDBInfo[2].substr(9, strlen(SplitDBInfo[2].c_str()) - 9);
  std::string DBUser = SplitDBInfo[3].substr(5, strlen(SplitDBInfo[3].c_str()) - 5 );

  std::cout << "DBIP: " << DBIP << std::endl;
  std::cout << "DBName: " << DBName << std::endl;
  std::cout << "DBPwd: " << DBPwd << std::endl;
  std::cout << "DBUser: " << DBUser << std::endl;

  /***End Retrieving Information from the Config File***/
  //get an instance of DNSNameserver
  Nameserver::DNSNameserver* mDNSServer = Nameserver::DNSNameserver::GetInstance();

  mDNSList->setDatabaseIP(DBIP); 
  mDNSList->setDatabseUser(DBUser);	
  mDNSList->setDatabasePass(DBPwd);	
  mDNSList->setDatabaseName(DBName);

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
    try{
      std::vector <int> statusParser;
      int RBL = 1;
      statusParser.push_back(RBL);
      statusParser.push_back(0); statusParser.push_back(0);
      std::cout << "Before ret" << std::endl;
      //call the function to start the nameserver
      int ret = mDNSServer->StartNameServer( PortArray[0], MainServerIPAddress, statusParser);
	std::cout << "After ret" << std::endl;
	delete mDNSServer;
	return ret;
    }catch(...){
      std::cout << "Process created for PortNumber: " << PortArray[0] << "Failed: " << std::endl;
    }
  }	  
  else {//parent process
    pid_t  pidPar;
    pidPar = fork();
    if (pidPar == 0){
      std::vector <int> statusParser;
      int MALWARE = 1;
      statusParser.push_back(0);statusParser.push_back(MALWARE);statusParser.push_back(0);
      //RBL =false; MALWARE =true; CHILD = false;
      try{
	//call the function to start the nameserver
	int ret = mDNSServer->StartNameServer( PortArray[1], MainServerIPAddress, statusParser);
	delete mDNSServer;
	return ret;
      }catch(...){
	std::cout << "Process Created for PortNumber: " << PortArray[1] << "Failed: " << std::endl;
      }
    }		  
    else{
      std::vector <int> statusParser;
      int CHILD;
      statusParser.push_back(0);statusParser.push_back(0);statusParser.push_back(CHILD);
      //RBL = false; MALWARE = false; CHILD = true;
      try{
	//call the function to start the nameserver
	int ret = mDNSServer->StartNameServer( PortArray[2], MainServerIPAddress, statusParser );
	delete mDNSServer;
	return ret;
      }catch(...){
	std::cout << "Process Created for PortNumber: " << PortArray[2] << "Failed: " << std::endl;
      }
    }		  
  }	
  return 1;
}//end main
