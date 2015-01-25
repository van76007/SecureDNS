/**
* @file   forwarder.cpp
* @author dvv <dvv@avauntguard.com>
* @brief  A forwarder to replay packet to DNS server
*/

#include "forwarder.h"
#include "dns_util.h"

namespace screenDNS {

	forwarder::forwarder() {

	}

	void forwarder::print_debug_info(std::string debug_text)
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

	void forwarder::relay(
		boost::array<char, 512>& packet, std::size_t packet_len,
		boost::array<char, 512>* recv_buf, 
		size_t* recv_len) 
	{
		print_debug_info("Normal thread initiated ... ");
		*recv_len = 0;

		// Replay request to DNS server. Using native socket to send/receive with time out
		boost::asio::io_service DNS_io_service;
		udp::resolver resolver(DNS_io_service);

		// Nameserver IP
		std::string DNS_IP1 = "192.168.231.20";
		std::string DNS_IP2 = "192.168.231.23";		

		udp::resolver::query query1(udp::v4(), DNS_IP1, "53");
		udp::resolver::query query2(udp::v4(), DNS_IP2, "53");

		udp::endpoint receiver_endpoint1 = *resolver.resolve(query1);
		udp::endpoint receiver_endpoint2 = *resolver.resolve(query2);

		udp::socket DNS_socket(DNS_io_service);
		DNS_socket.open(udp::v4());

		try {

			DNS_socket.send_to(boost::asio::buffer(packet, packet_len), receiver_endpoint1);  
		        DNS_socket.send_to(boost::asio::buffer(packet, packet_len), receiver_endpoint2);  

		} catch (std::exception& e) {
			std::cout << "forwarder::relay_send_to throws error: " << e.what() << std::endl;
			return;
		}

		// Set up a timed select call, so we can handle timeout cases.
		fd_set fileDescriptorSet;
		struct timeval timeStruct;

		// set the timeout to 1 seconds
		timeStruct.tv_sec = 2;
		timeStruct.tv_usec = 0;
		FD_ZERO(&fileDescriptorSet);

		// We'll need to get the underlying native socket for this select call, in order
		// to add a simple timeout on the read:
		int nativeSocket = DNS_socket.native();

		FD_SET(nativeSocket,&fileDescriptorSet);
		select(nativeSocket+1,&fileDescriptorSet,NULL,NULL,&timeStruct);

		if(!FD_ISSET(nativeSocket,&fileDescriptorSet)){ 
			// timeout
			std::cout << "Read from DNS server timeout. Sent packet_len is " << packet_len << "\n";
		} else {

			udp::endpoint sender_endpoint;

			// Blocking receive from
			try {

				*recv_len = DNS_socket.receive_from(
					boost::asio::buffer(*recv_buf, 512), 
					sender_endpoint);

			} catch (std::exception& e) {

				std::cout << "forwarder::relay_receive_from throws error: " << e.what() << std::endl;
				return;
			}

			// Close the socket
			DNS_socket.close();			
		}
	}

	// Idea: void, passing (endpoint) so the code can resend the packet itself: 
	// Add 1 worker thread to query DNS server and get response
	// If we can send back DNS response directly in monitor_thread
	void forwarder::wait_condition_replay(
		boost::array<char, 512>& packet, 
		std::size_t packet_len,
		boost::shared_ptr<udp::socket> socket_ptr, 
		udp::endpoint remote_endpoint)
	{
		flag = false; // Reset for this run

		boost::asio::io_service work_io_service;

		boost::thread_group threads;
		size_t thread_group_size = 6;

		// Response buffer
		size_t recv_buf_len;
		boost::array<char, 512> recv_buf;

		// Post monitor services
		work_io_service.post(boost::bind(&screenDNS::forwarder::monitor,
			this,
			// Wait for 1s
			2000, 
			packet,
			packet_len, 
			socket_ptr, 
			remote_endpoint)
			);

		// Post rbl requests
		for (std::size_t i = 0; i < thread_group_size - 3; ++i) {
			work_io_service.post(
				boost::bind(&screenDNS::forwarder::find_fact,
				this,
				// Passing packet and packet_len 
				packet, 
				packet_len, 
				i));
		}

                // Post normal requests
		work_io_service.post(
			boost::bind(&screenDNS::forwarder::relay,
			this,
			packet, 
			packet_len,
			&recv_buf,
			&recv_buf_len)
			);		

		// Schedule thread
        	for (std::size_t i = 0; i < thread_group_size; ++i)
   	    		threads.create_thread(boost::bind(&boost::asio::io_service::run, &work_io_service));

		work_io_service.reset();
		work_io_service.run();
		threads.join_all();
                //work_io_service.stop();

		// Nothing blocked or timeout occured on the RBL threads
		// Then send the normal response
		if(!flag)
		{
			if (recv_buf_len > 0) {

				socket_ptr->async_send_to(
					boost::asio::buffer(recv_buf, recv_buf_len),  
					remote_endpoint,
					boost::bind(&screenDNS::forwarder::handle_send,
					this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred)
					);
			}		
		}		
	}

	void forwarder::monitor(int timeout,
		boost::array<char, 512>& packet, 
		std::size_t packet_len, 
		boost::shared_ptr<udp::socket> socket_ptr, 
		udp::endpoint remote_endpoint) {

			print_debug_info("Monitor thread initiated ... ");

			// mutex_lock while(!flag) {cond_wait_with_timeout} mutex_unlock
			boost::mutex::scoped_lock lock(mut); // boost::unique_lock<boost::mutex> lock(mut);
			completed_rbl_queries = 0;

			// Loop until timeout or flag = T. Some other thread may reset flag to T before
			bool done = false;
			while(!done && !flag && completed_rbl_queries < 3) {

				// Version1: cond.timed_wait: Not good yet as not wait for lock but wait till time out
				if (!cond.timed_wait(lock, boost::posix_time::milliseconds(timeout)))
				{
					print_debug_info("Time out occured ... ");
					done = true;
				} else {

				print_debug_info("Got Notified ...");

                                }
			}			

			// this allows us to quickly block any page
			if(flag)
			{
				unsigned char buffer[packet_len];

				// Convert from packet to buffer
				for (int i=0; i<packet_len; i++)
					buffer[i] = packet[i];

				std::string TTL("0.0.1.44");
				std::string IP("202.123.2.6"); // CA 7B 02 06

				// Alloc memory for crafted DNS response
				std::size_t data_len = packet_len + 16; 				             
				unsigned char* data = (unsigned char*)calloc(data_len, sizeof(unsigned char)); 

				craft_DNS_response(buffer, packet_len, TTL, IP, data);

				socket_ptr->async_send_to(
					boost::asio::buffer(data, data_len),  
					remote_endpoint,
					boost::bind(&screenDNS::forwarder::handle_send,
					this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred)
					); 
			}
	}

	// Dont do any print out here or deadlock
	void forwarder::find_fact(boost::array<char, 512>& packet, std::size_t packet_len, int id) {

		print_debug_info("RBL thread initiated ... ");		
		
		unsigned char buffer[packet_len];
		bool blocked = false;

		std::string suffix("dnsbl.sorbs.net");				

		switch (id) {

			case 0 :
				suffix = "rhsbl.ahbl.org"; 
				break;
			case 1 :
				suffix = "dnsbl.sorbs.net";
				break;
			case 2 :
				suffix = "multi.surbl.org";
				break;
		}		

		// Convert from packet to buffer
		for (int i=0; i<packet_len; i++)
			buffer[i] = packet[i];

		size_t rbl_query_len = 0;
		unsigned char* rbl_query = (unsigned char*)calloc(512, sizeof(unsigned char));	

		// Create a request buffer corresponding to the current RBL query    
		craft_DNS_query(buffer, packet_len, suffix, rbl_query, rbl_query_len);

		// boost representation of the request buffer
		boost::array<char, 512> rbl_query_packet;

		for (int i=0; i < rbl_query_len; i++)
			rbl_query_packet[i] = rbl_query[i];

		// Relay packet
		size_t rbl_recv_len;
		boost::array<char, 512> rbl_recv_packet;

		// Forward to the nameservers
		relay(rbl_query_packet, rbl_query_len, &rbl_recv_packet, &rbl_recv_len);		

		if (rbl_recv_len > 0)
		{
			unsigned char DNS_resp_packet[rbl_recv_len];

			for (int i=0; i<rbl_recv_len; i++)
				DNS_resp_packet[i] = rbl_recv_packet[i];

			// Parse packet 
			bool resp_ok = true; // To know if response packet if malformatted?
			struct DNS_Packet* resp_packet = DNS_parse_packet(DNS_resp_packet, rbl_recv_len, resp_ok);

			// Analyze resp_packet and decide if this is a block response. Condition: Get 1 ANSWER
			if (resp_packet->ancount)
			{		
				print_debug_info("Received one answer from RBL.");
				print_debug_info(suffix);
				
				blocked = true;
			}

			DNS_free_packet(resp_packet); 
		}					

		// Must notify monitor thread that operation is completed
		// And that it can proceed to block the user		
		boost::mutex::scoped_lock lock(mut);			

		if(!flag && completed_rbl_queries < 3)
		{
			if(blocked)
				flag = true;
			else
				completed_rbl_queries++;

			// If we do not notify monitor thread when all our RBL threads have executed
			// then the monitor thread will run for 2 seconds before sending response O_o
			if(flag || completed_rbl_queries >= 3)
				cond.notify_one();			
		}

		// Free resource
		free(rbl_query);       
	}

	// To be invoked after sending packet
	void forwarder::handle_send(const boost::system::error_code& error,
		std::size_t bytes_transferred)
	{
		boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();

		if (!error && bytes_transferred > 0) {

			global_stream_lock.lock();
			printf("DEBUG :: %s :: Successfully received and forwarded response ...\n", to_simple_string(now).c_str());		
			global_stream_lock.unlock();
		} 
		else {

			global_stream_lock.lock();
			printf("ERROR :: %s :: Could not forward response ...\n", to_simple_string(now).c_str());		
			global_stream_lock.unlock();
		}
	}

	void forwarder::test(int* count) {
		++(*count);
	}
}

