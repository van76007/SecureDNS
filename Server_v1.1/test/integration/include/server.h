/**
* @file   server.h
* @author dvv <dvv@avauntguard.com>
* @brief  A relay DNS server using Boost Asio threadpool
*/
#ifndef __SERVER_H__
#define __SERVER_H__

#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/array.hpp>
// For using native socket solution
#include <sys/time.h>
// For measuring time performance
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include "forwarder.h"

using namespace boost::posix_time;


#define MSG_LEN 512

using boost::asio::ip::udp;

namespace screenDNS {

	class server : private boost::noncopyable {
	public:
		// Constructor
		explicit server(int port, std::size_t thread_pool_size);
		void run();
		~server();

	private:
		// The number of threads that will call io_service::run().
		std::size_t thread_pool_size_;
		// The io_service used to perform asynchronous operations.
		boost::asio::io_service io_service_;

		// For UDP socket
		boost::asio::io_service sock_io_service;
		//udp::socket socket_;
		boost::shared_ptr<udp::socket> socket_ptr; // Ptr to socket can be passed to forwarder object

		// Issue: Each read must have its own copy of remote endpoint and recv_buffer
		udp::endpoint remote_endpoint_;
		boost::array<char, 512> recv_buffer_;

		// To protect shared resource std::cout
		boost::mutex global_stream_lock;

		// Function to print debug text
		void print_debug_info(std::string debug_text);

		// Start receive packets
		void start_receive();

		// Handle received packets
		void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred);

		// Process received packets
		void process_packet(boost::array<char, 512>& packet, 
			std::size_t packet_len, 
			udp::endpoint remote_endpoint);

		// Handle request to send packet
		void handle_send(const boost::system::error_code& error, std::size_t bytes_transferred);

		// Replay request to DNS server
		/*
		* @ToDo: Using boost_share_ptr or boost_scope_ptr to implement forwarder
		* http://www.boost.org/doc/libs/1_47_0/libs/smart_ptr/shared_ptr.htm
		* http://www.boost.org/doc/libs/1_46_1/libs/smart_ptr/example/shared_ptr_example2.cpp
		* Boost_2 tutorial. Constructor: T(new T)
		*/
		boost::shared_ptr<screenDNS::forwarder> myForwarder;    
	};

}

#endif /* __SERVER_H__ */

