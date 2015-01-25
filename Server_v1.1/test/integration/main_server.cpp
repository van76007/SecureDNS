#include <iostream>
#include <boost/lexical_cast.hpp>
#include "server.h"

void start_server(int port, 
		  std::size_t num_threads)
{
	screenDNS::server s(port, num_threads);
	s.run();
}

void init(std::size_t thread_pool_size)
{
	boost::asio::io_service main_io_service;
	boost::thread_group threads;
	size_t thread_group_size = 3;	

	// CAI lookup thread
	main_io_service.post(boost::bind(start_server,
				53,
				thread_pool_size)
				);

	// Malware lookup thread
	main_io_service.post(boost::bind(start_server,
				20053,
				thread_pool_size)
				);

	// RBL lookup thread
	main_io_service.post(boost::bind(start_server,
				30053,
				thread_pool_size)
				);

	for (std::size_t i = 0; i < thread_group_size; ++i)
   		threads.create_thread(boost::bind(&boost::asio::io_service::run, &main_io_service));

	main_io_service.reset();
	main_io_service.run();
	
	threads.join_all();
}

int main(int argc, char* argv[]){

	try
	{		
		// Check command line arguments.
		if (argc < 1)
		{
			std::cerr << "Usage: test_server <threads>\n";
			return 1;
		}

		// Initialise the server.
		std::size_t num_threads = boost::lexical_cast<std::size_t>(argv[1]);		
	
		init(num_threads);
	}
	catch (std::exception& e)
	{
		std::cout << "exception: " << e.what() << "\n";
	}

	return 0;
}
