#include "Transfer.h"

namespace screenDNS {

    Transfer::Transfer(string pipeName)
             : m_pipeName(pipeName)
    {}
    
    bool Transfer::init()
    {
		// Create the named - pipe
        int ret_val = mkfifo(m_pipeName.c_str(), 0666);

        if ((ret_val == -1) && (errno != EEXIST)) {
            cout << "Error creating the named pipe" << endl;
            return false;
        }
        
        // Open the pipe for reading. Will be blocked here until someone writting to the pipe!!
        fd = open(m_pipeName.c_str(), O_RDONLY|O_NONBLOCK);
				
		if (fd>0)
		    return true;
		else
		    return false;
		
		//return true;
    }
    
    vector<string> Transfer::parseMessage(string message)
    {
		std::vector <std::string> SplitNewLine;
		std::vector <std::string> strs;
		
		boost::split(SplitNewLine, message, boost::is_any_of("\n"));
		
		if((int)SplitNewLine.size() >= 1)
			 boost::split(strs, SplitNewLine[0], boost::is_any_of("("));       
       
        return strs;
    }
	
    string Transfer::buildQ(string table_name,
                            string FQDN,    			// requested URL
                            string domain,    			// what is found in Blacklist DB
							string IP,
                            string unified_response,  	//
                            string zone_name, 			// may be null
                            string create_date 			// the moment we write log to the pipe
                      )
    {
        /*
        INSERT INTO blocked_domains(blocked_domains.domain, blocked_domains.blocked_domain, blocked_domains.unified_response, blocked_domains.zone_name, blocked_domains.create_date) values(
		'google.com', -- BLOCKED DOMAIN
		'www.google.com', -- REQUESTED SITE
		'127.0.0.1', -- Unified Response
		null, -- Zone name
		'2011-09-23' -- DATE
		)
        */
        ostringstream statement;
        
        // Push it statement << HARDCODED_STR << VARIABLES ;
        statement << "INSERT INTO " << table_name
                  << "("
				  << table_name << ".domain, "
	              << table_name << ".blockeddomain, "
				  << table_name << ".domainip, "	
				  << table_name << ".response, "
				  << table_name << ".zone_name, "
				  << table_name << ".createdate)"
                  << " VALUES('"
                  << domain << "', '"
                  << FQDN << "', '"
				  << IP << "', '"
                  << unified_response << "', '"
                  << zone_name << "', '"
				  << create_date << "')";
		
        return statement.str();
    }
    
	string Transfer::buildQWhiteList(string table_name,
						   string hostname, 
						   string country_code, 
						   string notes, 
						   string source	
						)
	{
	    /*
        INSERT IGNORE INTO whitelist(hostname, country_code, notes, source)
        VALUES('www.gmail.com', '230', 'cname whitelist', 'screenDNS')		
        */
		ostringstream statement;
		
		// Push it statement << HARDCODED_STR << VARIABLES ;
        statement << "INSERT IGNORE INTO " << table_name
                  << "("
				  << table_name << ".hostname, "
	              << table_name << ".country_code, "
				  << table_name << ".notes, "	
				  << table_name << ".source)"
                  << " VALUES('"
                  << hostname << "', '"
                  << country_code << "', '"
				  << notes << "', '"
				  << source << "')";
		
		return statement.str();
	}
	
    MYSQL* Transfer::createconnection(string dbip, string dbuser,
                                      string dbpwd, string dbname)
    {
	MYSQL *conn;
	conn = mysql_init(NULL);

	if(conn == NULL){
		cout << "Could not initialize mysql_init" << endl;
		mysql_close(conn);
		return NULL;
	}

	/* Connect to database. Error is obtained in string mysql_error(conn) */
	if (!mysql_real_connect(conn, dbip.c_str(), dbuser.c_str(), 
                                dbpwd.c_str(), dbname.c_str(), 0, NULL, 0))
            mysql_close(conn);

	return conn;
    }

    MYSQL_RES* Transfer::doquery(const char *query, MYSQL *conn)
    {
        MYSQL_RES *res=NULL;

  	/* send SQL query */
  	if (mysql_query(conn, query))
            return res;

  	res = mysql_use_result(conn);

  	return res;
    }
    
    void Transfer::closeconnection(MYSQL *conn)
    {
        mysql_close(conn);
        mysql_library_end();
    }

    void Transfer::operator () () {
		
        while (true) {
			
            try {
                
                // Read from the pipe
                int numread;
                char buf[MAX_BUF_SIZE];
                if (fd > 0)
                    numread = read(fd, buf, MAX_BUF_SIZE); // #define MAX_BUF_SIZE	512

                if (numread > 0) {

                    //Parse message
                    string chunk(buf, numread);
					size_t found = chunk.find_last_of("(");
					string message = "";
					if (found != -1)
					    message = chunk.substr(0,found);
                    
					vector<string> components = parseMessage(message);
					
					MYSQL* conn = NULL;
					conn = mysql_init(conn);
					if( NULL != conn) {
						
						string dbip = Config::Instance()->get_dbip(); // "192.168.230.138"
						string dbuser = Config::Instance()->get_dbuser(); // "root"
						string dbpwd = Config::Instance()->get_dbpwd(); // "ulaysiP801"
						string dbname = Config::Instance()->get_dbname(); // "secdns_dev"
						
						conn = createconnection(dbip, dbuser, dbpwd, dbname);
						if( NULL != conn) {
							// Prepare Data
							string table_name = Config::Instance()->get_table_name(); // "blocked_domains"
							string whitelist_table_name = Config::Instance()->get_whitelist_table_name(); // "whitelist"
							
							string query = "";
							if( components.size()==6 ) {
								// Build query string for logging to table "blockeddomainstats"
								string blocked_domain = components[0];
								string FQDN  = components[1];
								string IP = components[2];
								string unified_response = components[3];
								string zone_name = components[4];
								string create_date = components[5];
								
								query = buildQ(table_name,
											  blocked_domain,    // requested URL
											  FQDN,
											  IP,
											  unified_response,
											  zone_name,
											  create_date
											  );
							} else if( components.size()==4 ) {
								// Build query string for logging to table "whitelist"
								string hostname = components[0];
								string country_code = components[1];
								string notes = components[2];
								string source = components[3];
								
								query = buildQWhiteList(whitelist_table_name,
														hostname, 
														country_code, 
														notes, 
														source	
														);
							}
												  
							// Insert logged data
							#ifdef DBUG
							cout << query << endl;
							#endif
							
							// Comment out when debugging
							if ( mysql_real_query(conn, query.c_str(), strlen(query.c_str())) && (query.size() != 0))
								fprintf(stderr, "%s\n", mysql_error(conn));	
						} // End if( NULL != conn)
					
						// Close connection
						closeconnection(conn);
					} // End if( NULL != conn)
				} // End if (numread > 0)
				
				// Prevent 100% CPU load
                usleep(10); // usleep(microseconds) - 1000 microseconds in a millisecond 
				
                // Check if this thread is interrupted by main
		        boost::this_thread::interruption_point();
            } catch (const boost::thread_interrupted&) {
                cout << "Get thread interrupted. Exiting..." << endl;
                // @ToDo: Close SQL connection to remote database
                break;
            } // End try ... catch ...
			
        } // End while(true)
     
    }   
}
