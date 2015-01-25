#include <iostream>
#include <mysql/mysql.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include <stdio.h>
#include <ctime>
#include <cdb.h>
#include <boost/lexical_cast.hpp>

#include "Config.h"

using namespace screenDNS;

MYSQL* createconnection(std::string dbip, std::string dbuser, std::string dbpwd, std::string dbname)
{
	MYSQL *conn;
	conn = mysql_init(NULL);

	if(conn == NULL){
		std::cout << "Could not initialize mysql_init" << std::endl;
		mysql_close(conn);
		//throw 1;
	}

	std::cout << "/*********************************/" << std::endl;
	std::cout << " * ip  : " << dbip  << std::endl;
	//std::cout << " * user: " << dbuser << std::endl;
	//std::cout << " * pwd : ***** " << std::endl;
	std::cout << " * db  : " << dbname << std::endl;
	std::cout << "/*********************************/" << std::endl;

	/* Connect to database */
	if (!mysql_real_connect(conn, dbip.c_str(), dbuser.c_str(), dbpwd.c_str(), dbname.c_str(), 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		mysql_close(conn);		
	}

	return conn;
}

MYSQL_RES* doquery(const char *query, MYSQL *conn)
{
  	MYSQL_RES *res = NULL;

  	/* send SQL query */
  	if (mysql_query(conn, query)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		return res;
  	}
  	res = mysql_use_result(conn);

  	return res;
}

bool build_cdb(std::string cdb_path, std::string query, std::string dbip, std::string dbuser, std::string dbpwd, std::string dbname)
{
	struct cdb_make cdbm;
	int fd;
	bool retcode = false;
	
	std::string key, val;
	int key_len, val_len;
     	unsigned int totalKeylen = 0, totalVallen = 0;	

	MYSQL* conn = NULL;
     	conn = mysql_init(conn);

	MYSQL_RES* res;
     	MYSQL_ROW row;
	
     	if( NULL == conn) {

		std::cout << "Database connection failed."  << std::endl;		

     	}//end if statement
	else {

		printf("Generating file : '%s'\n", (unsigned char*)cdb_path.c_str());	
		
		conn = createconnection(dbip, dbuser, dbpwd, dbname);		
		fd = open(cdb_path.c_str(), O_RDWR|O_CREAT, S_IRWXU | S_IRGRP | S_IROTH); /* open temporary file */

        	if(fd < 0) {
			std::cout << "Failed to create cdb at the given path:" << cdb_path << std::endl;
			return false;
		}

		cdb_make_start(&cdbm, fd); /* initialize structure */
		res = doquery(query.c_str(), conn); /* res will contain the data for our cdb file */

		while ((row = mysql_fetch_row(res)) != NULL) {
      		
			key = row[0];
      			val = row[1];

      			key_len = key.size();
     			val_len = val.size();
 
			totalKeylen += (unsigned int) key_len;
			totalVallen += (unsigned int) val_len;
			
			/* add as many records as needed */
			cdb_make_add(&cdbm, key.c_str(), key_len, val.c_str(), val_len);	      
    		}

		// free resources
		mysql_free_result(res);

		/* final stage - write indexes to CDB file */
		cdb_make_finish(&cdbm); 

		printf("Successfully generated file : '%s'\n", (unsigned char*)cdb_path.c_str());
		retcode = true;		
	}

	// Close connections	
	mysql_close(conn);
   	mysql_library_end();

	return retcode;
}

std::string get_cdb_suffix(std::string dblog_filename)
{
	std::string suffix = "";

	FILE* fp;
	char buffer[50];

    fp = fopen((const char *)dblog_filename.c_str(),"r");

	// File does not exist
	if(fp == NULL)
	{
		printf("File does not exist : %s\n", dblog_filename.c_str());
        suffix = "1";		
	} else {
	    fgets(buffer, 50, fp);
		std:: string text(buffer);
		if (text == Config::Instance()->get_log_content_1())// "F1 is in use"
			suffix = "2";
		else
			suffix = "1";
		fclose(fp);
	}
	
	return suffix;
}

void update_log(std::string dblog_filename, std::string increment)
{
	FILE* fp;
	char buffer[50];

	fp = fopen((const char *)dblog_filename.c_str(),"r");

	// File does not exist, create it
    if(fp == NULL)
    {
        printf("File does not exist : %s\n", dblog_filename.c_str());

		// create the file		
		fp = fopen((const char *)dblog_filename.c_str(),"w+");		
    }
    fclose(fp);
	
	// File should exist by now
	// So overwrite it with the relevant data	
	fp = fopen((const char *)dblog_filename.c_str(),"w+");
	if(fp != NULL)
	{
		if (increment == "2")
		{
			printf("New value contained in log : 'F2 is in use'\n\n");
			fputs(Config::Instance()->get_log_content_2().c_str(), fp);
		}
		else if (increment == "1")
		{
			printf("New value contained in log : 'F1 is in use'\n\n");
			fputs(Config::Instance()->get_log_content_1().c_str(), fp);
		}		
	}
	
	fclose(fp);
}

void generate_cdb(int cdb_type, std::string configFilePath)
{
	// Get the config parameters from the config file
	std::string dbip = Config::Instance()->get_cdb_dbip(); // "192.168.231.56";
	std::string dbuser = Config::Instance()->get_cdb_dbuser(); // "admin" or "cdb";
	// @ToDo: Change password excludes # and use Config class instead. Reason, Boost program_options misidentifies #mark as comment sign
	std::string dbpwd = Config::Instance()->get_cdb_dbpwd(); // "screenDNS#11" or "cdb123"
	std::string dbname = Config::Instance()->get_cdb_dbname(); // "screenDNS";
	
	// Variables

	std::string query = "";	
	std::string log_filename = "";
	std::string cdb_filename = "";	
	std::string prefix = "";

	switch(cdb_type) {	

        case 1:
            log_filename = Config::Instance()->get_cdbDirPath() + "/" + Config::Instance()->get_dbLogFile_childporn(); 
			prefix = Config::Instance()->get_dbFileName_childporn();			
			query = "	SELECT distinct LOWER(cai.hostname), '127.3.255.224' FROM cai";
			break;

		case 2:

			log_filename = Config::Instance()->get_cdbDirPath() + "/" + Config::Instance()->get_dbLogFile_malware(); 
			prefix =  Config::Instance()->get_dbFileName_malware();			
			query = "	SELECT distinct LOWER(malware.hostname), malware.unified_response FROM malware WHERE malware.unified_response != '' AND malware.unified_response IS NOT NULL";
			break;

		case 3:

			log_filename = Config::Instance()->get_cdbDirPath() + "/" + Config::Instance()->get_dbLogFile_ip(); 
			prefix = Config::Instance()->get_dbFileName_ip();
			query = "	SELECT distinct blocked_ips.ip, blocked_ips.unified_response FROM blocked_ips WHERE blocked_ips.unified_response != '' AND blocked_ips.unified_response IS NOT NULL";
			break;

		case 4:

			log_filename = Config::Instance()->get_cdbDirPath() + "/" + Config::Instance()->get_dbLogFile_whitelist(); 
			prefix = Config::Instance()->get_dbFileName_whitelist();		
			query = "	SELECT distinct LOWER(hostname), country_code from whitelist WHERE whitelist.is_active = 1 order by country_code ASC, hostname ASC";				
			break;

		default:
			printf("Invalid database type ... \n");
			break;
	}

	if(	query != "" 
		&& log_filename != ""
		&& prefix != "")
	{
		std::string suffix = get_cdb_suffix(log_filename);
		cdb_filename = Config::Instance()->get_cdbDirPath() + "/" + prefix + suffix + ".cdb";
        
		std::cout << "log_filename=" << log_filename << std::endl;
		std::cout << "cdb_filename=" << cdb_filename << std::endl;
		
		if(build_cdb(cdb_filename, query, dbip, dbuser, dbpwd, dbname))
			update_log(log_filename, suffix);
	}
}
/*
sudo ./cdb_generator 5 /usr/local/etc/configDNS/config.ini
*/
int main(int argc, char* argv[]) {		

	if (argc != 3) {

		printf(" **********************************************\n");
		printf(" * Usage is e.g. sudo ./CDB 1 <absolute_path_2_config_file>\n");
		printf(" * This will generate either child1.cdb or child2.cdb\n\n");
		printf(" * Option 1 is cdb type \n");
		printf(" * 1. Child \n");
		printf(" * 2. Malware \n");
		printf(" * 3. Blocked \n");
		printf(" * 4. Whitelist \n\n");
		printf(" * 5. Generate all \n\n");
		printf(" **********************************************\n");		
	}
	else {
	
        std::string configFilePath = string(argv[2], strlen(argv[2]));
		int selection = boost::lexical_cast<int>(argv[1]);
		std::cout << "configFilePath=" << configFilePath << std::endl;
		std::cout << "selection=" << selection << std::endl;
		
		//Initialize Config
	    Config::Instance()->parseConfigFile(configFilePath);
	    Config::Instance()->elaborate();
		
		if(selection != 5)
		{
			// Generate 1 category
			generate_cdb(selection, configFilePath);
		} else if(selection == 5){
		    // Generate all categories
			for(int i=1; i<5; i++) {
				
				printf("Generating all CDBs ... \n");
				generate_cdb(i, configFilePath);
			}
		}			
	}	

	return 1;
}




