/**
* @file   forwarder.h
* @author dvv <dvv@avauntguard.com>
* @brief  A forwarder to replay packet to DNS server
* In put: packet, socket, remote_endpoint
* Action: send back packet
* Question: Should we use scoped_ptr or share_ptr
* Test: dig @192.168.230.80 -p 2000 www.security.com +tries=0 
* Or  : dig @192.168.230.80 -p 2000 newsletterupdatecenterline.com 
*/
#ifndef __FORWARDER_H__
#define __FORWARDER_H__

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread_time.hpp>
#include <boost/thread/locks.hpp>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

// For random generator
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

// For measuring time performance
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

// For using native socket solution
#include <sys/time.h>

using boost::asio::ip::udp;

namespace screenDNS {

	class forwarder {

	public:

		explicit forwarder();

		// Replay packet to normal DNS server
		// V.2: void function, passing reference to 
		void relay(
			// input
			boost::array<char, 512>& packet, std::size_t packet_len,
			// output
			boost::array<char, 512>* recv_buf,
			size_t* recv_len);

		// Replay packets to RBLs
		void wait_condition_replay(
			// input
			boost::array<char, 512>& packet, std::size_t packet_len,
			boost::shared_ptr<udp::socket> socket_ptr, 
			udp::endpoint remote_endpoint);

	private:

		// To protect shared resource std::cout
		boost::mutex global_stream_lock;

		// boolean value to indicate whether something
		// has been detected as blocked amongst the RBLs queries
		bool flag;

		// integer value to track the number of completed RBL threads
		// when this object reaches max_rbl_thread_count, code should break
		int completed_rbl_queries;
 
		// Synchronization primes. Usage: cond.timed_wait(lock, timeout)
		boost::mutex mut;
		boost::condition_variable cond;

		// Function to print debug text
		void print_debug_info(std::string debug_text);

		void monitor(	int timeout, 
				boost::array<char, 512>& packet, 
				std::size_t packet_len,
				boost::shared_ptr<udp::socket> socket_ptr, 
				udp::endpoint remote_endpoint);

		void find_fact(boost::array<char, 512>& packet, std::size_t packet_len, int id);

		// Handle request to send packet
		void handle_send(const boost::system::error_code& error, std::size_t bytes_transferred);

		// Test parsing parameter by reference.
		/* Usage:
		int count = 0;
		test(&count);
		*/
		void test(int* count);
	};

}

#endif /* __FORWARDER_H__ */
