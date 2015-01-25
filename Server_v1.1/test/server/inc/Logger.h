/**
 * @file   Logger.h
 * @author dvv <dvv@avauntguard.com>
 * @brief  Implement logging using pipe
 */
#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>

#define MAX_BUF_SIZE	512

using namespace std;

// Version 2: Singleton
namespace screenDNS {
	class Logger{
	public:
	   static Logger* Instance();
	   
	   void init(string pipeName);
	   
		// If block result come from DB, zone_name is *null*
		// If block result come from RBL, unified_response is IP_from_RBL,
		// domain = blocked_domain
		void log(
				string domain,    // requested URL
				string blocked_domain,    // what is found in Blacklist DB
				string IP, // IP of blocked_domain
				string unified_response,  // Element of vector found in Blacklist
				string zone_name, // may be null
				string create_date // the moment we write log to the pipe
				);
		
		// Log for whitelist table
		void logWhiteList(
						string hostname, 
						string country_code, 
						string notes, 
						string source	
						);
					 
		void close(); 

	private:
	   Logger(){}; // 
	   Logger(Logger const&){};             // copy constructor is private
	   Logger& operator=(Logger const&){};  // assignment operator is private
	   static Logger* m_pInstance;
	   
	   string m_pipeName;
	   int fd;
	};
}

#endif /* __LOGGER_H__ */
