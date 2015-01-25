/**
 * @file   forwarder.h
 * @author dvv <dvv@avauntguard.com>
 * @brief  A forwarder to replay packet to DNS server
 * In put: packet, socket, remote_endpoint
 * Action: send back packet
 * Question: Should we use scoped_ptr or share_ptr
 * Test: dig @192.168.230.83 -p 2000 www.security.com +tries=0
 * Or  : dig @192.168.230.83 -p 2000 newsletterupdatecenterline.com 
 */
#ifndef __FORWARDER_H__
#define __FORWARDER_H__

#include <boost/asio.hpp>
#include <boost/bind.hpp>
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
#include <ctime>

// For convert character to lower case
#include <algorithm>

// For extracting information from request/response
#include "dns_util.h"

// Prevent re-definition of classes "DBManager" in "Reader" class
#define MAIN
#include "DBManager.h"
#include "Reader.h"
#include "Logger.h"

using boost::asio::ip::udp;
using namespace std;
using namespace screenDNS; 

namespace screenDNS {

class forwarder {

    public:

        explicit forwarder();
        
        ~forwarder();

        void setupReader(DBManager* manager_IP, DBManager* manager_MALWARE, 
                   DBManager* manager_CHILDPORN, DBManager* manager_WHITELIST);
        
        // Replay packets after checking on White & Black lists
        void relay_back(
                    // input
                    boost::array<char, 512>& packet, size_t packet_len,
                    // let us know from which port the packet come from so we can treat differently 
                    int tag,
                    // This function will send back the response itself after processing request
                    boost::shared_ptr<udp::socket> socket_ptr, 
                    udp::endpoint remote_endpoint);

        // Replay packets after checking by RBLs
        void relay_to_RBL(
                    int tag,
                    boost::array<char, 512>& packet, size_t packet_len,
                    boost::shared_ptr<udp::socket> socket_ptr, 
                    udp::endpoint remote_endpoint);

    private:
        
        // To provide lookup service
        Reader* m_reader_IP;
        Reader* m_reader_MALWARE;
        Reader* m_reader_CHILDPORN;
        Reader* m_reader_WHITELIST;
		
		// For logging purpose
		string zone; // Name of RBL site
        string IP_from_RBL; // IP in the answer section of DNS response from RBL
		
        // To protect shared resource cout. Can be removed
        boost::mutex global_stream_lock;
        
		// Synchronization primes. Usage: cond.timed_wait(lock, timeout)
        boost::mutex mut;
        boost::condition_variable cond;
        
		// Vars need to be protected by mutex
        bool flag, sent_block_resp;
		// integer value to track the number of completed RBL threads
	    // when this object reaches max_rbl_thread_count, code should break
	    int completed_rbl_queries;

        // Function to print debug text
		void print_debug_info(string debug_text);

        // Function to create time stamp
        string getTimeStamp();
		
		// Function get all sub-strings of FQDN/CNAME
		vector<string> getSubs(string Name);
		
		// Function to verify vector in Whitelist CDB
		bool verify_vector_InWhite(vector<string>& input);
		
		// Function to verify vector in Blacklist CDB
		bool verify_vector_InBlack(int tag, // What kind of lookup: CAI, Malice, etc...
								vector<string>& input, // All the strings need to be lookup in CDB
								string& blocked_domain, // Could be FQDN/sub-FQDN
								vector<string>& uni_resp // All the uni_resp strings found in CDB 
								);
		
		// Function to verify domain string in Blacklist CDB	
		bool verify_string_InBlack(int tag, // What kind of lookup: CAI, Malice, etc...
								string domain, // String need to be lookup in CDB
								vector<string>& uni_resp // All the uni_resp strings found in CDB 
								);
								
        // Function to verify DNS request
		bool verify_request(bool* p_ok,
							int tag, unsigned char* buf, size_t buf_len, // Input: Tag_port, packet, packet_length
							string& FQDN, // FQDN from Question[] section of Request packet
							string& blocked_domain, // Could be FQDN/sub-FQDN
							vector<string>& uni_resp // All the uni_resp strings corresponding to blocked_domain 
							);
		
        // Function to verify DNS response
        bool verify_response(bool* p_ok,
							 bool* p_CNAMEInWhiteList, // Reserved to use for the auto-whitelist feature
							 bool* p_IPInBlackList,	// To flag if IP is blocked
							 int tag, unsigned char* buf, size_t buf_len,
							 string IP, // IP from normal response
							 vector<string>& IPs, // IPs need for logging
							 string& blocked_domain, // Could be blocked_IP or blocked_sub_CNAME
							 vector<string>& uni_resp // All the uni_resp strings corresponding to blocked_domain
							 );
		
		// 1st log: To get block reason
		void doLoggingReason(string blocked_domain, string FQDN, vector<string> uni_resp);
		
		// 2nd log: To do statistics task
		void doLoggingStat(string blocked_domain, string FQDN, vector<string> IPs, vector<string> uni_resp);
		
		// To get block reason for RBL
		void doLoggingForRBL(string FQDN, string normal_IP, string IP_from_RBL, string zone);
		
		// To auto-whiteList
		void doLoggingForWhiteList(string hostname, string country_code, string notes, string source);
		
		// Relay block response
        void sendBlockResponse(unsigned char* buffer, size_t packet_len,
                               boost::shared_ptr<udp::socket> socket_ptr,
                               udp::endpoint remote_endpoint);

        // Relay packet to normal DNS server
        void relay(
                   // input
                   boost::array<char, 512>& packet, size_t packet_len,
                   // output
                   boost::array<char, 512>* recv_buf,
                   size_t* recv_len);

        void monitor(int timeout,
		            size_t num_workers,
                    unsigned char* buffer, size_t packet_len,
		            boost::shared_ptr<udp::socket> socket_ptr, 
                    udp::endpoint remote_endpoint);
 
        void find_fact(boost::array<char, 512>& packet, size_t packet_len,
                       string suffix);
        
        // Handle request to send packet. Standard version
        void handle_send(const boost::system::error_code& error, 
                         size_t bytes_transferred);

        // Handle request to send packet. Extended version. 
        // Free allocated buffer for packet after sending
        void handle_send_ng(const boost::system::error_code& error, 
                           size_t bytes_transferred,
                           unsigned char* pData);

};

}

#endif /* __FORWARDER_H__ */
