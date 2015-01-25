#include "Config.h"

namespace pod = boost::program_options::detail;

namespace screenDNS {

    // Global static pointer used to ensure a single instance of the class.
	Config* Config::m_pInstance = NULL;  

	/** This function is called to create an instance of the class. 
		Calling the constructor publicly is not allowed. The constructor 
		is private and is only called by this Instance function.
	*/
	Config* Config::Instance()
	{
	   if (!m_pInstance)   // Only allow one instance of class to be generated.
		  m_pInstance = new Config;

	   return m_pInstance;
	}
	
	void Config::setTxt()
	{
	    m_txt = "BigFoo";
	}
	
	string Config::getTxt()
	{
	    return m_txt;
	}
	
	void Config::elaborate()
	{
	    cout << "log_content_1: " << log_content_1 << "\n";
		cout << "log_content_2: " << log_content_2 << "\n";
		cout << "dbFileName_ip: " << dbFileName_ip << "\n";
		cout << "dbFileName_malware: " << dbFileName_malware << "\n";
		cout << "dbFileName_childporn: " << dbFileName_childporn << "\n";
		cout << "dbFileName_whitelist: " << dbFileName_whitelist << "\n";

		// forwarder.cpp
		cout << "TTL: " << TTL << "\n";
		cout << "block_IP: " << block_IP << "\n";
		cout << "DNS_IP1: " << DNS_IP1 << "\n";
		cout << "DNS_IP2: " << DNS_IP2 << "\n";

		// main_server.cpp
		cout << "cdbDirPath: " << cdbDirPath << "\n";
		cout << "transferLog: " << transferLog << "\n";
		cout << "port_protection_plus_1: " << port_protection_plus_1 << "\n"; // 10053
		cout << "port_protection_plus_2: " << port_protection_plus_2 << "\n"; // 20053
		cout << "port_protection_plus_3: " << port_protection_plus_3 << "\n"; // 30053

		// Monitor.cpp
		cout << "dbLogFile_ip: " << dbLogFile_ip << "\n";
		cout << "dbLogFile_malware: " << dbLogFile_malware << "\n";
		cout << "dbLogFile_childporn: " << dbLogFile_childporn << "\n";
		cout << "dbLogFile_whitelist: " << dbLogFile_whitelist << "\n";

		// Transfer.cpp
		cout << "dbip: " << dbip << "\n";
		cout << "dbuser: " << dbuser << "\n";
		cout << "dbpwd: " << dbpwd << "\n";
		cout << "dbname: " << dbname << "\n";
		cout << "table_name: " << table_name << "\n";
		cout << "whitelist_table_name: " << whitelist_table_name << "\n";
		
		// CDB Generator
		cout << "cdb_dbip: " << cdb_dbip << "\n";
		cout << "cdb_dbuser: " << cdb_dbuser << "\n";
		cout << "cdb_dbpwd: " << cdb_dbpwd << "\n";
		cout << "cdb_dbname: " << cdb_dbname << "\n";
	}
	
	bool Config::parseConfigFile(string configFile)
	{
	    // @ToDo: Parse YAML config file here
		// Parsing parameter from main_server to constructor of Config
		// configFile = "/usr/local/etc/configDNS/config.yaml"
		
		// Key strings in config.ini
		string key_port_protection_plus_1("port.port_protection_plus_1");
		string key_port_protection_plus_2("port.port_protection_plus_2");
		string key_port_protection_plus_3("port.port_protection_plus_3");
		string key_TTL("block_response.TTL");
        string key_block_IP("block_response.block_IP");
		string key_DNS_IP1("DNS_Server.DNS_IP1");
        string key_DNS_IP2("DNS_Server.DNS_IP2");
		string key_transferLog("logging.transferLog");
		string key_dbip("database.dbip");
        string key_dbuser("database.dbuser");
        string key_dbpwd("database.dbpwd");
        string key_dbname("database.dbname");
        string key_table_name("database.table_name");
		string key_whitelist_table_name("database.whitelist_table_name");
		string key_cdbDirPath("cdb_dir.cdbDirPath");
		string key_dbFileName_ip("cdb_file.dbFileName_ip");
        string key_dbFileName_malware("cdb_file.dbFileName_malware");
        string key_dbFileName_childporn("cdb_file.dbFileName_childporn");
        string key_dbFileName_whitelist("cdb_file.dbFileName_whitelist");
		string key_log_content_1("dblog_content.log_content_1");
        string key_log_content_2("dblog_content.log_content_2");
        string key_dbLogFile_ip("dblog.dbLogFile_ip");
        string key_dbLogFile_malware("dblog.dbLogFile_malware");
        string key_dbLogFile_childporn("dblog.dbLogFile_childporn");
        string key_dbLogFile_whitelist("dblog.dbLogFile_whitelist");
		string key_cdb_dbip("cdb_database.dbip");
        string key_cdb_dbuser("cdb_database.dbuser");
        string key_cdb_dbpwd("cdb_database.dbpwd");
        string key_cdb_dbname("cdb_database.dbname");
		string key_list_RBLs("RBL.list_RBLs");
		string key_socket_to("timeout.socket_to");
		string key_condition_var_to("timeout.condition_var_to");
		
		ifstream config(configFile.c_str());
        if(!config) {
		    cerr<<"Error opening config file: "<< configFile <<endl;
		    return false;
		}
		
		//parameters
		set<string> options;
		//map<string, string> parameters;
		options.insert("*");
		
		try
		{      
			for (pod::config_file_iterator i(config, options), e ; i != e; ++i)
			{
				if (i->string_key == key_log_content_1)
				    log_content_1 = i->value[0];
				
				if (i->string_key == key_log_content_2)
				    log_content_2 = i->value[0];
				
				if (i->string_key == key_dbFileName_ip)
				    dbFileName_ip = i->value[0];
				
				if (i->string_key == key_dbFileName_malware)
				    dbFileName_malware = i->value[0];
				
				if (i->string_key == key_dbFileName_childporn)
				    dbFileName_childporn = i->value[0];
				
				if (i->string_key == key_dbFileName_whitelist)
				    dbFileName_whitelist = i->value[0];
				
				if (i->string_key == key_TTL)
				    TTL = i->value[0];
				
				if (i->string_key == key_block_IP)
				    block_IP = i->value[0];
				
				if (i->string_key == key_DNS_IP1)
				    DNS_IP1 = i->value[0];
				
				if (i->string_key == key_DNS_IP2)
				    DNS_IP2 = i->value[0];
				
				if (i->string_key == key_cdbDirPath)
				    cdbDirPath = i->value[0];
				
				if (i->string_key == key_transferLog)
				    transferLog = i->value[0];
				
				if (i->string_key == key_port_protection_plus_1)
				    port_protection_plus_1 = i->value[0];
				
				if (i->string_key == key_port_protection_plus_2)
				    port_protection_plus_2 = i->value[0];
				
				if (i->string_key == key_port_protection_plus_3)
				    port_protection_plus_3 = i->value[0];
				
				if (i->string_key == key_dbLogFile_ip)
				    dbLogFile_ip = i->value[0];
				
				if (i->string_key == key_dbLogFile_malware)
				    dbLogFile_malware = i->value[0];
					
				if (i->string_key == key_dbLogFile_childporn)
				    dbLogFile_childporn = i->value[0];

                if (i->string_key == key_dbLogFile_whitelist)
				    dbLogFile_whitelist = i->value[0];
                
                if (i->string_key == key_dbip)
				    dbip = i->value[0];

                if (i->string_key == key_dbuser)
				    dbuser = i->value[0];
                
                if (i->string_key == key_dbpwd)
				    dbpwd = i->value[0];
                
                if (i->string_key == key_dbname)
				    dbname = i->value[0];
				
				if (i->string_key == key_cdb_dbip)
				    cdb_dbip = i->value[0];

                if (i->string_key == key_cdb_dbuser)
				    cdb_dbuser = i->value[0];
                
                if (i->string_key == key_cdb_dbpwd)
				    cdb_dbpwd = i->value[0];
                
                if (i->string_key == key_cdb_dbname)
				    cdb_dbname = i->value[0];

                if (i->string_key == key_table_name)
				    table_name = i->value[0];
					
				if (i->string_key == key_whitelist_table_name)
				    whitelist_table_name = i->value[0];

                if (i->string_key == key_list_RBLs)
				    list_RBLs = i->value[0];
					
				vector<string> strs;
				boost::split(strs, list_RBLs, boost::is_any_of(";"));
				num_RBLs = strs.size();
				
                if (i->string_key == key_socket_to)
				    socket_to = i->value[0];

                if (i->string_key == key_condition_var_to)
				    condition_var_to = i->value[0];					
			}
			//cout << parameters["port_protection_plus_1"] << endl;
		}
		catch(exception& e)    
		{
			cerr<<"Exception: "<<e.what()<<endl;
			return false;
		}
		
        return true;
	}
}
