/**
 * @file   Transfer.h
 * @author dvv <dvv@avauntguard.com>
 * @brief  Read from logging pipe and write to remote SQL database.
 * @ref http://developers.sun.com/solaris/articles/named_pipes.html
 */
#ifndef __TRANSFER_H__
#define __TRANSFER_H__

#include <string>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <map>
#include <boost/thread.hpp>
// To split message
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
// To connect to SQL
#include <mysql/mysql.h>
// To build SQL statement in an elegant way
#include <sstream>
#include <iostream>

#include "Config.h"

// Max size of message read from a pipe. See code of Logger
#define MAX_BUF_SIZE	512

using namespace std;

namespace screenDNS {

    class Transfer {
    public:
        // Constructor
        Transfer(string pipeName);
        
        // Create named pipe for communication
        bool init();

        // Callable from another thread
        void operator () ();

    private:
        string m_pipeName;
        int fd; // File descriptor of the named pipe

        vector<string> parseMessage(string message);
		
        string buildQ(string table_name,
                      string blocked_domain,    // requested URL
                      string FQDN,    // what is found in Blacklist DB
					  string IP,
                      string unified_response,  // Element of vector found in Blacklist
                      string zone_name, // may be null
                      string create_date // the moment we write log to the pipe
                    );
					  
		string buildQWhiteList(string table_name,
							   string hostname, 
							   string country_code, 
							   string notes, 
							   string source	
							);

        MYSQL* createconnection(string dbip, string dbuser,
                                string dbpwd, string dbname);

        MYSQL_RES* doquery(const char *query, MYSQL *conn);

        void closeconnection(MYSQL *conn);
    };
}

#endif /* __TRANSFER_H__ */
