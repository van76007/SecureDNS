#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <mysql/mysql.h>
#include <yaml.h>
#include <vector>

//Sufficient for 10 entries in the table
#define BLEN 5420
#define LEN 512

//function to parse the config file
int ParseConfig(std::string ConfigPath,
				std::vector<std::string> & SplitDatabaseInfo) {

					FILE *FileHandler = fopen(ConfigPath.c_str(), "r");
					yaml_parser_t Yamlparser; 
					yaml_event_t Yamlevent, YE;

					if(!yaml_parser_initialize(&Yamlparser)){
						return 0;// MainException("Yaml parser Initialization Failure");
					}

					if( NULL == FileHandler){
						return 0;//MainException( "Config File Handler Exception");
					}

					yaml_parser_set_input_file(&Yamlparser, FileHandler);
					std::string DatabaseInfo;

					do{
						//Call YAML parser
						yaml_parser_parse(&Yamlparser, &Yamlevent);

						if(Yamlevent.type == YAML_SCALAR_EVENT) {

							std::string actual((char*)Yamlevent.data.scalar.value);
							//determine the Info for the Database
							std::string Database = "Database";
							if( strcmp( actual.c_str(), Database.c_str() ) ) {
							}else{
								yaml_parser_parse(&Yamlparser, &YE);
								DatabaseInfo = (char*)YE.data.scalar.value; 
							}
							if(YE.type != YAML_STREAM_END_EVENT){
								yaml_event_delete(&Yamlevent);
								yaml_event_delete(&YE);	
							}
						}
						//std::cout << "Within While loop"  << std::endl;    
					}while(Yamlevent.type != YAML_STREAM_END_EVENT);//end while lop
					std::cout << "DatabaseInfo: \n" << DatabaseInfo << std::endl;
					boost::split(SplitDatabaseInfo, DatabaseInfo, boost::is_any_of(" "));
					yaml_event_delete(&Yamlevent);  
					yaml_parser_delete(&Yamlparser);
					fclose(FileHandler);
					return 1;
}//end parseConfig


//Function to write to the database
int writeDat(std::vector<std::string> SplitDBInfo, std::string compleDom, 
			 std::string domain, std::string client_ip, std::string domain_ip,
			 std::string response, std::string zoneName){
				 std::string dbip = SplitDBInfo[0].substr(3, strlen(SplitDBInfo[0].c_str()) - 3);
				 std::string dbname = SplitDBInfo[1].substr(5, strlen(SplitDBInfo[1].c_str()) - 5);
				 std::string dbpwd = SplitDBInfo[2].substr(9, strlen(SplitDBInfo[2].c_str()) - 9);
				 std::string dbuser = SplitDBInfo[3].substr(5, strlen(SplitDBInfo[3].c_str()) - 5 );

				 if(compleDom.empty() || domain.empty()) return 1;

#ifdef DEBUG
				 std::cout << "dbip: " << dbip << std::endl;
				 std::cout << "dbname: " << dbname << std::endl;
				 std::cout << "dbpwd: " << dbname << std::endl;
				 std::cout << "dbuser: " << dbname << std::endl;
				 std::cout << "ZN in writelog: " << zoneName << std::endl;
				 std::cout << "client_ip: " << client_ip << std::endl;
				 std::cout << "compleDom" << compleDom << std::endl;
#endif
				 std::cout << "\nDatabase: " << dbip << std::endl;

				 MYSQL* connectionDat;
				 connectionDat = mysql_init(NULL);

				 if( NULL == connectionDat){
					 mysql_close(connectionDat);
					 mysql_library_end();
					 return 1;
				 }

				 /* Connect to database */
				 printf("Connecting to database ... \n");
				 if (NULL == mysql_real_connect(connectionDat, dbip.c_str(), dbuser.c_str(), dbpwd.c_str(), dbname.c_str(), 0, NULL, 0)) {  
					 fprintf(stderr, "%s\n", mysql_error(connectionDat));
					 mysql_close(connectionDat);
					 mysql_library_end();
					 return 1;
				 }
				 printf("Connected to database ... \n");

				 char query[LEN];
				 memset(&query, '\0', LEN);
				 int query_length = sizeof("INSERT INTO blockeddomainstats (clientip, domain, blockeddomain, domainip, response, zone_name) values('");

				 strncat(query,"INSERT INTO blockeddomainstats (clientip, domain, blockeddomain, domainip, response, zone_name) values('", query_length);
				 strncat(query, client_ip.c_str(), strlen(client_ip.c_str()) );

				 //log the blocked domain
				 strncat(query, "','", 3);
				 strncat(query, domain.c_str(), strlen(domain.c_str()));

				 if( !compleDom.compare(" ") ) return 0;

				 //log the complete domain
				 strncat(query, "','", 3);
				 strncat(query, compleDom.c_str(), strlen(compleDom.c_str()));

				 strncat(query, "','", 3);
				 strncat(query, domain_ip.c_str(), strlen(domain_ip.c_str()));

				 strncat(query, "','", 3);
				 strncat(query, response.c_str(), strlen(response.c_str()));

				 strncat(query, "','", 3);
				 //add the zoneName
				 strncat(query, zoneName.c_str(), strlen(zoneName.c_str()));
				 strncat(query, "')", 2);

				 std::cout << "Executing query: \n" << query << std::endl;

				 // send SQL query
				 if (mysql_query(connectionDat, query)) {
					 //fprintf(stderr, "%s\n", mysql_error(connectionDat));
					 std::cout << "Could Not input to the database the log info: - writetolog" << std::endl;
					 mysql_close(connectionDat);
					 mysql_library_end();
					 return 1;
				 }
				 else
					 printf("Query executed successfully ... \n");

				 mysql_close(connectionDat);
				 mysql_library_end();

				 printf("Exiting ... \n");

				 return 0;
}

int main( int argc, char* argv[] ){

	if(argc > 2 || argc < 2){
		std::cout << "Config Path missing: E.g. <path to ReadPipe> PipeConf.yaml" << std::endl;
		return 1;
	}   
	std::vector<std::string> SplitDBInfo;
	std::string PathConfig(argv[1]);
	int ret = ParseConfig(PathConfig, SplitDBInfo);
	if (ret  == 0){
		std::cout << "Could not parse the PipeConf.yaml file" << std::endl;
		return 1;	
	}
	int fd, ret_val, numread;
	char bufpipe[BLEN];
	std::vector <std::string> SplitOutput;
	std::vector <std::string> SplitNewLine;
	//string bufpipe;
	while(1){
		ret_val = mkfifo("Blocked_Domains", 0666);	
		if (( ret_val == -1) && (errno != EEXIST)) 
		{
			perror("Pipe Creation Failed");
			exit(1);
		}
		fd = open("Blocked_Domains", O_RDONLY);
		if(fd < 1){
			std::cout << "FD failed" << std::endl;
			return 1;	
		}
		numread = read(fd,bufpipe, BLEN);
		bufpipe[numread] = '\0';
		// std::cout << "Bufpipe: '" << bufpipe << "'" << std::endl;
		// split new lines
		boost::split(SplitNewLine, bufpipe, boost::is_any_of("\n"));

		for(int y = 0; y < (int)SplitNewLine.size(); y++){
			boost::split(SplitOutput, SplitNewLine[y], boost::is_any_of("("));	

			if(SplitOutput.size() > 0){
				if (SplitOutput.size() % 6 == 0) {
					writeDat(SplitDBInfo, SplitOutput[0], SplitOutput[1], SplitOutput[2], SplitOutput[3], SplitOutput[4], SplitOutput[5]);
				}
			}
			SplitOutput.clear();
		}		
		memset(bufpipe,'\0',BLEN +1);
	}
}
