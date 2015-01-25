#include <iostream>
#include <boost/lexical_cast.hpp>

#include "server.h" // Already define "DBManager.h" & "Reader.h" & "Logger.h"
#define MAIN // Prevent re-definition of classes "DBManager" in "Reader" and "Monitor" classes
#include "Monitor.h"
#include "Transfer.h"
#include "Config.h"

void start_server(int port, size_t thread_pool_size,
                  DBManager* manager_IP, DBManager* manager_MALWARE, 
                  DBManager* manager_CHILDPORN, DBManager* manager_WHITELIST)
{
	screenDNS::server s(port, thread_pool_size,
                            manager_IP, manager_MALWARE, 
                            manager_CHILDPORN, manager_WHITELIST);
	s.run();
}

// ToDo: + Init 3 DBManager objects
//       + Start 3 monitor threads here to monitor the changes of .cdb files
//       + Init 3 Reader objects to provide lookup service to forwarder 
void init(size_t thread_pool_size, string configFilePath)
{
	boost::asio::io_service main_io_service;
	boost::thread_group threads;
	size_t thread_group_size = 3;
	
    Config::Instance()->parseConfigFile(configFilePath);
	Config::Instance()->elaborate();
	
    // Step 0: Init DB Managers, Readers and start Monitoring services
    // DB_Managers
    string cdbDirPath = Config::Instance()->get_cdbDirPath(); // "/usr/home/dvv"; // Test machine .199

	DBManager* manager_IP = new DBManager();
	manager_IP->setupDB(cdbDirPath, IP);

	DBManager* manager_MALWARE = new DBManager();
	manager_MALWARE->setupDB(cdbDirPath, MALWARE);

	DBManager* manager_CHILDPORN = new DBManager();
	manager_CHILDPORN->setupDB(cdbDirPath, CHILDPORN);

	DBManager* manager_WHITELIST = new DBManager();
	manager_WHITELIST->setupDB(cdbDirPath, WHITELIST);

	// Start 4 monitoring threads
	screenDNS::Monitor monitor_IP(manager_IP);
	screenDNS::Monitor monitor_MALWARE(manager_MALWARE);
	screenDNS::Monitor monitor_CHILDPORN(manager_CHILDPORN);
	screenDNS::Monitor monitor_WHITELIST(manager_WHITELIST);

	boost::thread t_monitor_IP(monitor_IP);
	boost::thread t_monitor_MALWARE(monitor_MALWARE);
	boost::thread t_monitor_CHILDPORN(monitor_CHILDPORN);
	boost::thread t_monitor_WHITELIST(monitor_WHITELIST);
	
	// Wait 5s till monitor signals DBManager loads data into memory
	sleep(10);
	
	// Step 1: Setup logging mechanism
	// Start Transfer in a thread: Server-side of half-duplex pipe comm
	cout << "Starting transfer ..." << endl;
	screenDNS::Transfer transferLog("logScreenDNS");
	bool ok = transferLog.init();
	// If initialize named pipe and connect to SQL database OK
	boost::thread t_transferLog;
	if (ok)
		t_transferLog = boost::thread(transferLog);
	else
		cout << "Init transfer object is NOT OK" << endl;
	
	// Start Logger: Client-side of half-duplex pipe comm
	cout << "Done starting transfer. Init logger ..." << endl;
	Logger::Instance()->init("logScreenDNS");
	cout << "Done initializing logger ..." << endl;
    cout << "Num of RBLs=" << Config::Instance()->get_num_RBLs() << endl;

	// Step 2: Start servers listening in 3 ports 
	// Malware + CP + RBL lookup thread.
	main_io_service.post(boost::bind(start_server,
						atoi(Config::Instance()->get_port_protection_plus_1().c_str()), // 10053
						thread_pool_size,
                        manager_IP, manager_MALWARE, 
                        manager_CHILDPORN, manager_WHITELIST)
				);

	// Malware + CP lookup thread.
	main_io_service.post(boost::bind(start_server,
						atoi(Config::Instance()->get_port_protection_plus_2().c_str()), // 20053
						thread_pool_size,
                        manager_IP, manager_MALWARE, 
                        manager_CHILDPORN, manager_WHITELIST) 
				);

	// Only CP lookup thread.
	main_io_service.post(boost::bind(start_server,
						atoi(Config::Instance()->get_port_protection_plus_3().c_str()), // 30053
						thread_pool_size,
                        manager_IP, manager_MALWARE, 
                        manager_CHILDPORN, manager_WHITELIST)
				);

	for (size_t i = 0; i < thread_group_size; ++i)
   		threads.create_thread(boost::bind(&boost::asio::io_service::run,
                                                  &main_io_service));

	main_io_service.reset();
	main_io_service.run();
	
	threads.join_all();

	// Stop monitor threads
	cout << "Stop monitor threads..." << endl;
	t_monitor_IP.interrupt();
	t_monitor_MALWARE.interrupt();
	t_monitor_CHILDPORN.interrupt();
	t_monitor_WHITELIST.interrupt();

	// Stop transferring log thread
	t_transferLog.interrupt();

	// Delete objects
	cout << "Delete objects..." << endl;
	delete manager_IP;
	delete manager_MALWARE;
	delete manager_CHILDPORN;
	delete manager_WHITELIST;
}

int main(int argc, char* argv[]){

	size_t num_threads;
	string configFilePath = "";
	
	try
	{		
		// Check command line arguments.
		if (argc <= 2)
		{
			cerr << "Usage: ./main_server <num_of_threads> <absolute_path_2_config_file>\n";
			cerr << "Example: ./main_server 100 /usr/local/etc/configDNS/config.ini\n";
			cerr << "Due to lack of parameter, using default parameter: num_of_threads=50, absolute_path_2_config_file=/usr/local/etc/configDNS/config.ini\n";
			// Default values: 50 threads and default config file
			num_threads = 50;
			configFilePath = "/usr/local/etc/configDNS/config.ini";
		} else {
		    num_threads = boost::lexical_cast<size_t>(argv[1]);
			configFilePath = string(argv[2], strlen(argv[2]));
		}		
	    
		// Initialise the server		
		init(num_threads, configFilePath);
	}
	catch (exception& e)
	{
		cout << "exception: " << e.what() << "\n";
	}

	return 0;
}
