/**
* @file   server.cpp
* @author dvv <dvv@avauntguard.com>
* @brief  A relay DNS server using Boost Asio threadpool
*/

#include "server.h"

using boost::asio::ip::udp;

namespace screenDNS {

	server::server(	int port, 
		       	std::size_t thread_pool_size)
			// Initialisation
			:	thread_pool_size_(thread_pool_size),
			socket_ptr( new udp::socket(sock_io_service, udp::endpoint(udp::v4(), port)) ),
			myForwarder(new forwarder)
	{
		std::string port_val = boost::lexical_cast<std::string>(port);

		print_debug_info("Now listening on port " + port_val + " ... ");

		// Bind UDP socket
		start_receive();
	}

	server::~server() {
		socket_ptr->close();
	}   

	void server::print_debug_info(std::string debug_text)
	{
		#ifdef DBUG
		
		boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();		

		global_stream_lock.lock();

		#ifdef THREAD

		std::cout << "Thread ID :: [" << boost::this_thread::get_id() << "]" << "\n";

		#endif
		
		printf("DEBUG :: %s :: %s\n", to_simple_string(now).c_str(), debug_text.c_str());		
		global_stream_lock.unlock();

		#endif
	}

	void server::run() {

		// Create some work to stop io_service_.run() function from exiting if it has nothing else to do
		boost::shared_ptr< boost::asio::io_service::work> work(new boost::asio::io_service::work( io_service_ ));

		std::vector<boost::shared_ptr<boost::thread> > threads;

		boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&boost::asio::io_service::run, &sock_io_service)));
		threads.push_back(thread);

		for (std::size_t i = 0; i < thread_pool_size_; i++)
		{
			boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&boost::asio::io_service::run, &io_service_)));
			threads.push_back(thread);
		}

		// Wait for all threads in the pool to exit.
		for (std::size_t i = 0; i < threads.size(); ++i)
			threads[i]->join();
	}

	void server::start_receive() {			

		// Async receive data in background 
		socket_ptr->async_receive_from(
			boost::asio::buffer(recv_buffer_), 
			remote_endpoint_,
			boost::bind(&screenDNS::server::handle_receive,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)
			);
	}

	void server::handle_receive(const boost::system::error_code& error,
		std::size_t bytes_transferred) {

		print_debug_info("Packet received ... ");

			// io_service post work to thread
			io_service_.post(boost::bind(&screenDNS::server::process_packet, this, recv_buffer_, bytes_transferred, remote_endpoint_));

			// ready to receive packets again
			start_receive();
	}

	// Should make new object
	void server::process_packet(boost::array<char, 512>& packet, std::size_t packet_len, udp::endpoint remote_endpoint) {

		myForwarder->wait_condition_replay(packet, packet_len, socket_ptr, remote_endpoint);
	}

	// To be invoked after sent
	void server::handle_send(const boost::system::error_code& error,
		std::size_t bytes_transferred)
	{
		if (!error && bytes_transferred > 0)
		{
			global_stream_lock.lock();
			std::cout << "server::handle_send OK. bytes_transferred is " << bytes_transferred << "\n";
			global_stream_lock.unlock();
		} else {
			global_stream_lock.lock();
			std::cout << "server::handle_send ERROR. bytes_transferred is " << bytes_transferred << "\n";
			global_stream_lock.unlock();
		}
	}

}
