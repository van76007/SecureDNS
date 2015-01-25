#include <boost/filesystem.hpp>
#include <iostream>
#include <mysql/mysql.h>
#include <iostream>
#include <errno.h>
#include <string.h>

#include<sstream>
#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>
#include <boost/algorithm/string.hpp>
#include <yaml.h>

#include <ctime>
#include <cdb.h>

using namespace std;

std::string dbip;
std::string dbuser;
std::string dbpwd;
std::string dbname;
std::string RpathCDB;

MYSQL * createconnection()
{
	MYSQL *conn;
	//const char *server = "192.168.230.138";
	//const char *user = "root";
	//const char *password = "ulaysiP801";
	//const char *database = "secdns";
	//return 1;
	conn = mysql_init(NULL);
	if(conn == NULL){
		std::cout << "Could not initialize mysql_init" << std::endl;
		mysql_close(conn);
		//throw 1;
	}
	std::cout << " " << std::endl;
	std::cout << "/************/" << std::endl;
	std::cout << "Back"  << std::endl;
	std::cout << "dbip: " << dbip  << std::endl;
	std::cout << "dbuser: "  << dbuser << std::endl;
	std::cout << "dbpwd: " << dbpwd  << std::endl;
	std::cout << "dbname: " << dbname << std::endl;
	std::cout << "/************/" << std::endl;
	std::cout << " " << std::endl;
	/* Connect to database */
	if (!mysql_real_connect(conn, dbip.c_str(), dbuser.c_str(), dbpwd.c_str(), dbname.c_str(), 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		mysql_close(conn);
		//throw 1;
	}
	//std::cout << "Return connection" << std::endl;
	return conn;
}

MYSQL_RES* doquery(const char *query, MYSQL *conn)
{
  MYSQL_RES *res=NULL;

  /* send SQL query */
  if (mysql_query(conn, query)) {
	fprintf(stderr, "%s\n", mysql_error(conn));
	return res;
   // throw 1;//MainException("Could not Query MySql database");
  }
  res = mysql_use_result(conn);
  return res;
}//end do query


int buildWhiteCDB(std::string whiteDB){

	if( boost::filesystem::exists( whiteDB ) ){
		return 1;
	}

     struct cdb_make cdbm;
     int fd;
     char* val;
     int keylen, vallen;
     unsigned int totalKeylen = 0, totalVallen = 0;
     std::string key;
     MYSQL_RES *res;
     MYSQL_ROW row;

     MYSQL* conn = NULL;
     conn = mysql_init(conn);
     if( NULL == conn){
	std::cout << "Connection to Mysql Failed. Verify if the Mysql database info is correct in config file"  << std::endl;
	mysql_close(conn);
   	mysql_library_end();
  	 return 1;
     }//end if statement

     conn = createconnection();
	
    std::cout << "whiteDB: " << whiteDB << std::endl;
    //Whitelist
    fd = open(whiteDB.c_str(), O_RDWR|O_CREAT,S_IRWXU | S_IRGRP | S_IROTH); // open temporary file 
    if(fd < 0) {
		std::cout <<  "Failed to create whiteDB: " << whiteDB << std::endl;
		return 1;
	}
    cdb_make_start(&cdbm, fd); // initialize structure     

    std::string completeQueryWhitelist = "SELECT * FROM whitelist";
    res = doquery(completeQueryWhitelist.c_str(), conn);
    std::cout << "after res"  << std::endl;
    while ((row = mysql_fetch_row(res)) != NULL){
      key = row[0];
      val = row[1];
      keylen = key.size();  totalKeylen += (unsigned int) keylen;
      vallen =  strlen(val); totalVallen += (unsigned int) vallen;
      cdb_make_add(&cdbm, key.c_str(), keylen, val,vallen);      
    }
     cdb_make_finish(&cdbm);
     // Close file descriptor
     close(fd);
    mysql_close(conn);
    //free the results first
    mysql_free_result(res);
    return 0;
}

void buildMalCDB(std::string malDB){
	if( boost::filesystem::exists( malDB ) ){
		return;
	}

     struct cdb_make cdbm;
     int fd;
     char* val;
     int keylen, vallen;
     unsigned int totalKeylen = 0, totalVallen = 0;
     std::string key;
     MYSQL_RES *res;
     MYSQL_ROW row;
    
     MYSQL* conn = NULL;
     conn = mysql_init(conn);
     if( NULL == conn){
	std::cout << "Connection to Mysql Failed. Verify if the Mysql database info is correct in config file"  << std::endl;
	mysql_close(conn);
   	mysql_library_end();
  	 return;
     }
	conn = createconnection();

    std::cout << "MalDB: " << malDB << std::endl;
   //malware
    fd = open(malDB.c_str(), O_RDWR|O_CREAT,S_IRWXU | S_IRGRP | S_IROTH); // open temporary file 
    if(fd < 0) {
	std::cout << "Failed to create Malware cdb at the given path:" << malDB << std::endl;
	return;
	}
    cdb_make_start(&cdbm, fd); // initialize structure      

    std::string completeQueryMalwarelist = "(SELECT distinct malware.hostname, categories.unifiedresponse, malware.description FROM malware inner join categories on malware.category=categories.name)";//"SELECT hostname,unifiedresponse FROM malwareproduction";
    res = doquery(completeQueryMalwarelist.c_str(), conn);
    while ((row = mysql_fetch_row(res)) != NULL){
      key = row[0];
      val = row[1];
      std::stringstream inte (stringstream::in | stringstream::out);
      keylen = key.size();  totalKeylen += (unsigned int) keylen;
      vallen =  strlen(val); totalVallen += (unsigned int) vallen;
      cdb_make_add(&cdbm, key.c_str(), keylen, val,vallen);      
    }
     cdb_make_finish(&cdbm);
     // Close filedescriptor
     close(fd);
    //free the results first
    mysql_free_result(res);
}

void buildChildCDB(std::string childDB){

	if( boost::filesystem::exists( childDB ) ){
		return;
	}
   
     struct cdb_make cdbm;
     int fd;
     char* val;
     int keylen, vallen;
     unsigned int totalKeylen = 0, totalVallen = 0;
     std::string key;
     MYSQL_RES *res;
     MYSQL_ROW row;
     MYSQL* conn = NULL;
     conn = mysql_init(conn);
     if( NULL == conn){
	std::cout << "Connection to Mysql Failed. Verify if the Mysql database info is correct in config file"  << std::endl;
	mysql_close(conn);
   	mysql_library_end();
  	 return;
     }
	conn = createconnection();
    //Child
    fd = open(childDB.c_str(), O_RDWR|O_CREAT,S_IRWXU | S_IRGRP | S_IROTH); // open temporary file 
    if(fd < 0 ) {
		std::cout << "Failed to create childDB: " << childDB << std::endl;
		return;
	}
    cdb_make_start(&cdbm, fd); // initialize structure 
    cout << "db file open: " << fd << endl;
    cout << "Init db file"<< endl;

    std::string completeQueryChildlist = "SELECT distinct cai.hostname, categories.unifiedresponse, cai.source FROM cai inner join categories on cai.category=categories.name";//"SELECT hostname,unifiedresponse FROM caiproduction";
    res = doquery(completeQueryChildlist.c_str(), conn);
    while ((row = mysql_fetch_row(res)) != NULL){
      key = row[0];
      val = row[1];
      std::stringstream inte (stringstream::in | stringstream::out);
      keylen = key.size();  totalKeylen += (unsigned int) keylen;
      vallen =  strlen(val); totalVallen += (unsigned int) vallen;
      cdb_make_add(&cdbm, key.c_str(), keylen, val,vallen);      
    }
     cdb_make_finish(&cdbm);
     // Close file descriptor
     close(fd);
    //free the results first
    mysql_free_result(res);
}

void buildIPCDB(std::string IPDB){
		
	if( boost::filesystem::exists( IPDB ) ){
		return;
	}

	struct cdb_make cdbm;
     int fd;

     int keylen, vallen;
     unsigned int totalKeylen = 0, totalVallen = 0;
     std::string key;
     MYSQL_RES *res;
     MYSQL_ROW row;
     MYSQL* conn = NULL;
     conn = mysql_init(conn);
     if( NULL == conn){
	std::cout << "Connection to Mysql Failed. Verify if the Mysql database info is correct in config file"  << std::endl;
	mysql_close(conn);
   	mysql_library_end();
  	return;
     }  
	conn = createconnection();
    //IP    
    fd = open(IPDB.c_str(), O_RDWR|O_CREAT,S_IRWXU | S_IRGRP | S_IROTH); // open temporary file 
    if(fd < 0) {
		std::cout << "IP cdb failed: " << IPDB << std::endl;
		return;	
	}
    cdb_make_start(&cdbm, fd); // initialize structure 
  
    std::string completeQueryIPlist = "SELECT * FROM blocked_ips";
    res = doquery(completeQueryIPlist.c_str(), conn);
    
    while ((row = mysql_fetch_row(res)) != NULL){
      key = row[0];
	std::string val(row[1]);	
      std::stringstream inte (stringstream::in | stringstream::out);
      keylen = key.size();  totalKeylen += (unsigned int) keylen;
      vallen =  strlen(val.c_str()); totalVallen += (unsigned int) vallen;
      cdb_make_add(&cdbm, key.c_str(), keylen, val.c_str(), vallen);      
    }
     cdb_make_finish(&cdbm);
     // Close file descriptor
     close(fd);
    //free the results first
    mysql_free_result(res);
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


void readDB( std::string dbfile, std::string dupfile, std::string key ){
     int fd;
     int fddup;
     std::cout << "key: " << key << std::endl;
     // Open db file
     fd = open(dbfile.c_str(), O_RDONLY);
     if(fd == -1){
	  std::cout << "ERROR" << std::endl;
	  fddup = open(dupfile.c_str(), O_RDONLY);
	  if(fddup == -1){
		std::cout << "Failed to open: " << dupfile <<std::endl;
		return;
	 }else{
		fd = fddup;
		std::cout << "Worked out: " << dupfile << std::endl;
	}
	  return;
     } else {
	  cout << "OK" << endl;
     }


     std::string ValueString; ValueString.reserve(20);

	struct cdb cdb;
	char *val;
	unsigned vpos1, vlen1;

	cdb_init(&cdb, fd); // initialize internal structure 

  struct cdb_find cdbf; // structure to hold current find position 
  int err = cdb_findinit(&cdbf, &cdb, key.c_str(), key.size() ); // initialize search of key 
  if(err == 0) perror("findinit No more key");
  if(err < 0) perror("findinit Error");
  while(cdb_findnext(&cdbf) > 0) {
    vpos1 = cdb_datapos(&cdb);
    vlen1 = cdb_datalen(&cdb);
   // std::cout <<  "vlen: " << vlen1 << std::endl;
    val =(char*)malloc(vlen1 * sizeof(char*));
    cdb_read(&cdb, val, vlen1, vpos1);
    //std::cout << "VALUE: " << val << std::endl;
    // handle the value 
    free(val);
  }
  cdb_free(&cdb);
  // std::cout << "!val: " << val << std::endl;
  close(fd);
}


void monitorFile(int *times, std::string f1, std::string f2, int st){
	std::string mon;
	if( 1 == *times ){
		std::cout << "timewhi is: " << times << std::endl;
		if (1 == st){
			mon = "whi";		
		}
		else if( 2 == st){
			mon = "chi";
		}
		else if (3 == st){
			mon = "mal";
		}
		else if (4 == st){
			mon = "ipv";
		}
		if ( boost::filesystem::exists( mon ) )
		{	
			if ( (boost::filesystem::exists(f1)) && (boost::filesystem::exists(f2)) ){
				std::cout << "f 1 & 2 detected" << std::endl;
				*times = 0;
			}else if( boost::filesystem::exists(f2)  ){
				std::cout << "f 1 detected" << std::endl;
				buildWhiteCDB(f1);
				*times = 0;						
			}else{
				std::cout << "f detected" << std::endl;
				buildWhiteCDB(f2);
				*times = 0;
			}
			//create the cdb database
			std::cout <<  "File f Found" << std::endl;	
		}
	}
}

//function to parse the config file
int ParseConfig(std::string ConfigPath,
		 std::vector<std::string> & SplitDatabaseInfo){
  FILE *FileHandler = fopen(ConfigPath.c_str(), "r");
  yaml_parser_t Yamlparser; 
  yaml_event_t Yamlevent, YE;
  if(!yaml_parser_initialize(&Yamlparser)){
    return 0;
  }
  if( NULL == FileHandler){
    return 0;
  }
  yaml_parser_set_input_file(&Yamlparser, FileHandler);
  std::string DatabaseInfo;
  do{
    //Call YAML parser
    yaml_parser_parse(&Yamlparser, &Yamlevent);
    if(Yamlevent.type == YAML_SCALAR_EVENT){
      std::string actual((char*)Yamlevent.data.scalar.value);
      //determine the Info for the Database
      std::string Database = "Database";
      if( strcmp( actual.c_str(), Database.c_str() ) ) {
      }else{
	yaml_parser_parse(&Yamlparser, &YE);
	DatabaseInfo = (char*)YE.data.scalar.value; 
      }

      std::string PathCDB = "PathCDB";
      if( strcmp( actual.c_str(), PathCDB.c_str() ) ) {
      }else{
	yaml_parser_parse(&Yamlparser, &YE);
	RpathCDB = (char*)YE.data.scalar.value; 
      }


	if(YE.type != YAML_STREAM_END_EVENT){
		yaml_event_delete(&Yamlevent);
		yaml_event_delete(&YE);	
	}
    }    
  }while(Yamlevent.type != YAML_STREAM_END_EVENT);//end while lop
  std::cout << "DatabaseInfo: " << DatabaseInfo << std::endl;
  boost::split(SplitDatabaseInfo, DatabaseInfo, boost::is_any_of(" "));
  yaml_event_delete(&Yamlevent);  
  yaml_parser_delete(&Yamlparser);
  fclose(FileHandler);
  return 1;
}//end parseConfig


int main(int argc, char* argv[]) {
	if(argc > 2 || argc < 2){
		std::cout << "Config Path missing: E.g. <path to Mon> MonConf.yaml" << std::endl;
		return 1;
	}   
	std::vector<std::string> SplitDBInfo;
	std::string PathConfig(argv[1]);
	int ret = ParseConfig(PathConfig, SplitDBInfo);
	if (ret  == 0){
		std::cout << "Could not parse the MonConf.yaml file" << std::endl;
		return 1;	
	}
	std::cout << "converting the database info." << std::endl;
	dbip = SplitDBInfo[0].substr(3, strlen(SplitDBInfo[0].c_str()) - 3);
	dbname = SplitDBInfo[1].substr(5, strlen(SplitDBInfo[1].c_str()) - 5);
	dbpwd = SplitDBInfo[2].substr(9, strlen(SplitDBInfo[2].c_str()) - 9);
	dbuser = SplitDBInfo[3].substr(5, strlen(SplitDBInfo[3].c_str()) - 5 );

	//create the three databases upon restart of application
	std::string malwarecdb1 = RpathCDB + "/" + "malware1.cdb"; std::string malwarecdb2 = RpathCDB + "/" + "malware2.cdb"; std::string malwarecdb;
	std::string whitecdb1 = RpathCDB + "/" + "whitelist1.cdb"; std::string whitecdb2 = RpathCDB + "/" + "whitelist2.cdb"; std::string whitecdb;
	std::string childcdb1 = RpathCDB + "/" + "child1.cdb"; std::string childcdb2 = RpathCDB + "/" + "child2.cdb";std::string childcdb;
	std::string IPcdb1 = RpathCDB + "/" + "IP1.cdb"; std::string IPcdb2 = RpathCDB + "/" + "IP2.cdb";std::string IPcdb;
	std::cout << "malwarecdb1: " << malwarecdb1 << std::endl;
	std::cout << "malwarecdb2: " << malwarecdb2 << std::endl;
	std::cout << "whitecdb1: "  << whitecdb1 << std::endl;
	std::cout << "whitecdb2: " << whitecdb2 << std::endl;
	std::cout << "childcdb1: " << childcdb1 << std::endl;
	std::cout << "childcdb2: " << childcdb2 << std::endl;
	std::cout << "IPcdb1: " << IPcdb1 << std::endl;
	std::cout << "IPcdb1: " << IPcdb2 << std::endl;

	if(  !(boost::filesystem::exists( malwarecdb1 ) && boost::filesystem::exists(malwarecdb2)) ){
  		malwarecdb = malwarecdb1;
		if ( boost::filesystem::exists( malwarecdb1 ) ) {malwarecdb = malwarecdb2;}
		std::cout << "building Malware" << std::endl;
		buildMalCDB(malwarecdb);
		std::cout << "Finished Building Malware" << std::endl;
	}
        std::cout << "After Creation of Malware" << std::endl;
	if(  !(boost::filesystem::exists( childcdb1 ) && boost::filesystem::exists( childcdb2)) ){
		childcdb = childcdb1;
		if ( boost::filesystem::exists( childcdb1 ) ) { childcdb = childcdb2;}
		buildChildCDB(childcdb);
	}
	if(  !(boost::filesystem::exists( whitecdb1 ) && boost::filesystem::exists( whitecdb2 )) ){
		whitecdb = whitecdb1;
		if ( boost::filesystem::exists( whitecdb1 ) ) {whitecdb = whitecdb2;} 
		buildWhiteCDB(whitecdb);
	}
	if(  !(boost::filesystem::exists( IPcdb1 ) && boost::filesystem::exists( IPcdb2 )) ){
		IPcdb = IPcdb1;
		if ( boost::filesystem::exists( IPcdb1 )  ) {IPcdb = IPcdb2;}
		buildIPCDB(IPcdb);
	}
	//return 1;
/*	int timeswhi = 0;
	int timeschi = 0;
	int timesmal = 0;
	int timesIP = 0;
	while(1){
		monitorFile(&timeswhi, whitecdb1, whitecdb2, 1 );
		monitorFile(&timeschi, childcdb1, childcdb2, 2 );
		monitorFile(&timesmal, malwarecdb1, malwarecdb2, 3);
		monitorFile(&timesIP, IPcdb1, IPcdb2, 4 );
		//wait for 20 minutes after creation of a file
		sleep(3);	
		if(timeswhi > 1) timeswhi = 0;	
		timeswhi += 1;
		if(timeschi > 1) timeschi = 0;	
		timeschi += 1;
		if(timesmal > 1) timesmal = 0;	
		timesmal += 1;
		if(timesIP > 1) timesIP = 0;	
		timesIP += 1;
		std::cout << "timeswhi is: " << timeswhi << std::endl;
		std::cout << "timeschi is: " << timeschi << std::endl;
		std::cout << "timesmal is: " << timesmal << std::endl;
		std::cout << "timesIP is: " << timesIP << std::endl;
	}//end while loop
*/	return 1;
}
