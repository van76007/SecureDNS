#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "Logger.h" /* For name of the named-pipe */

namespace screenDNS {

    // Global static pointer used to ensure a single instance of the class.
	Logger* Logger::m_pInstance = NULL;  

	/** This function is called to create an instance of the class. 
		Calling the constructor publicly is not allowed. The constructor 
		is private and is only called by this Instance function.
	*/
	  
	Logger* Logger::Instance()
	{
	   if (!m_pInstance)   // Only allow one instance of class to be generated.
		  m_pInstance = new Logger;

	   return m_pInstance;
	}

    void Logger::init(string pipeName)
    {
        m_pipeName = pipeName;
        
        /* Open the pipe for writing */
        fd = open(m_pipeName.c_str(), O_WRONLY|O_NONBLOCK);
    }
	
	void Logger::close()
	{}
    
    void Logger::log(
                 string FQDN,    			// requested URL
                 string domain,    			// what is found in Blacklist DB
				 string IP, // IP of blocked_domain
                 string unified_response,  // 1 element of vector found in Blacklist
                 string zone_name, // may be empty
                 string create_date // the moment we write log to the pipe
                 )
    {
        	
        if (fd > 0)
		{
		    // Define seperator is "(" as this one is never seen in the domain
            string message = "";
			message += FQDN;
			message += "(";
			message += domain;
			message += "(";
			message += IP;
			message += "(";
			message += unified_response;
			message += "(";
			message += zone_name; // May be empty
			message +="(";
			message += create_date;
			message +="(";
						
			// Pad message to MAX_BUF_SIZE == 1024
			//cout << "Message length befor padding: " << message.size() << endl;
			size_t max_len = MAX_BUF_SIZE - message.size();
			for (int i = 0; i < max_len; i++)
			    message += ")";
			message += "\0";
			
			// Write message
            write(fd, message.c_str(), strlen(message.c_str()));
		}     
    }
	
	void Logger::logWhiteList(
							string hostname, 
							string country_code, 
							string notes, 
							string source	
							)
	{
	    if (fd > 0)
		{
		    // Define seperator is "(" as this one is never seen in the domain
            string message = "";
			message += hostname;
			message += "(";
			message += country_code;
			message += "(";
			message += notes;
			message += "(";
			message += source;
			message += "(";
			
			// Pad message to MAX_BUF_SIZE == 1024
			//cout << "Message length befor padding: " << message.size() << endl;
			size_t max_len = MAX_BUF_SIZE - message.size();
			for (int i = 0; i < max_len; i++)
			    message += ")";
			message += "\0";
			
			// Write message
            write(fd, message.c_str(), strlen(message.c_str()));
		}
	}							
}
