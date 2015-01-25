/**
 * @file   Config.h
 * @author dvv <dvv@avauntguard.com>
 * @brief  Implement Config Manager using singleton. This file must be copied exactly to CDB tool folder
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include <iostream>
#include <string>
#include <set>
#include <sstream>
#include <exception>
#include <fstream>

#include <boost/config.hpp>
#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/tokenizer.hpp> // To split string
#include <boost/algorithm/string.hpp> // To split string

using namespace std;

namespace screenDNS {
	class Config{
	public:
	    static Config* Instance();
	   
	    // Test
	    void setTxt();
	    string getTxt();
	    void elaborate();
	   
	    bool parseConfigFile(string configFile);
	   
		string get_log_content_1()
		{
			return log_content_1;
		}
		
		string get_log_content_2()
		{
			return log_content_2;
		}
		
		string get_dbFileName_ip()
		{
			return dbFileName_ip;
		}
		
		string get_dbFileName_malware()
		{
			return dbFileName_malware;
		}
		
		string get_dbFileName_childporn()
		{
			return dbFileName_childporn;
		}
		
		string get_dbFileName_whitelist()
		{
			return dbFileName_whitelist;
		}
		
		string get_TTL()
		{
			return TTL;
		}
		
		string get_block_IP()
		{
			return block_IP;
		}
		
		string get_DNS_IP1()
		{
			return DNS_IP1;
		}
		
		string get_DNS_IP2()
		{
			return DNS_IP2;
		}
		
		string get_cdbDirPath()
		{
			return cdbDirPath;
		}
		
		string get_transferLog()
		{
			return transferLog;
		}
		
		string get_port_protection_plus_1()
		{
			return port_protection_plus_1;
		}
		
		string get_port_protection_plus_2()
		{
			return port_protection_plus_2;
		}
		
		string get_port_protection_plus_3()
		{
			return port_protection_plus_3;
		}
		
		string get_dbLogFile_ip()
		{
			return dbLogFile_ip;
		}
		
		string get_dbLogFile_malware()
		{
			return dbLogFile_malware;
		}
		
		string get_dbLogFile_childporn()
		{
			return dbLogFile_childporn;
		}
		
		string get_dbLogFile_whitelist()
		{
			return dbLogFile_whitelist;
		}
		
		string get_dbip()
		{
			return dbip;
		}
		
		string get_dbuser()
		{
			return dbuser;
		}
		
		string get_dbpwd()
		{
			return dbpwd;
		}
		
		string get_dbname()
		{
			return dbname;
		}
		
		string get_table_name()
		{
			return table_name;
		}
		
		string get_whitelist_table_name()
		{
			return whitelist_table_name;
		}
		
		string get_cdb_dbip()
		{
			return cdb_dbip;
		}
		
		string get_cdb_dbuser()
		{
			return cdb_dbuser;
		}
		
		string get_cdb_dbpwd()
		{
			return cdb_dbpwd;
		}
		
		string get_cdb_dbname()
		{
			return cdb_dbname;
		}
		
		string get_list_RBLs()
		{
            return list_RBLs;		
		}
		
		size_t get_num_RBLs()
		{
		    return num_RBLs;
		}
		
		string get_socket_to()
		{
		    return socket_to;
		}
		
		string get_condition_var_to()
		{
		    return condition_var_to;
		}
	   
	private:
	   Config(){}; // 
	   Config(Config const&){};             // copy constructor is private
	   Config& operator=(Config const&){};  // assignment operator is private
	   static Config* m_pInstance;
	   
	   // Test
	   string m_txt;
	   
	   // DBManager.cpp
	   string log_content_1;
	   string log_content_2;
	   string dbFileName_ip;
	   string dbFileName_malware;
	   string dbFileName_childporn;
	   string dbFileName_whitelist;
	   
	   // forwarder.cpp
	   string TTL;
	   string block_IP;
	   string DNS_IP1;
	   string DNS_IP2;
	   
	   // main_server.cpp
	   string cdbDirPath;
	   string transferLog;
	   string port_protection_plus_1; // 10053
	   string port_protection_plus_2; // 20053
	   string port_protection_plus_3; // 30053
	   
	   // Monitor.cpp
	   string dbLogFile_ip;
	   string dbLogFile_malware;
	   string dbLogFile_childporn;
	   string dbLogFile_whitelist;
	   
	   // Transfer.cpp
	   string dbip;
	   string dbuser;
	   string dbpwd;
	   string dbname;
	   string table_name;
	   string whitelist_table_name;
	   string list_RBLs;
	   size_t num_RBLs;
	   string socket_to;
	   string condition_var_to;
	   
	   // CDB_Generator tool
	   string cdb_dbip;
	   string cdb_dbuser;
	   string cdb_dbpwd;
	   string cdb_dbname;
	};
}

#endif /* __CONFIG_H__ */
