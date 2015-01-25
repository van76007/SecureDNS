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

using namespace std;
using namespace boost::posix_time;
using boost::asio::ip::udp;

namespace screenDNS {

class server : private boost::noncopyable {
    public:
        // Constructor
        explicit server(int port, size_t thread_pool_size,
                        DBManager* manager_IP, DBManager* manager_MALWARE, 
                        DBManager* manager_CHILDPORN, DBManager* manager_WHITELIST);
        void run();
        ~server();

    private:

        // Port when server got packets from
        int m_port;

        // The number of threads that will call io_service::run().
        size_t thread_pool_size_;
        // The io_service used to perform asynchronous operations.
        boost::asio::io_service io_service_;

        // For UDP socket
        boost::asio::io_service sock_io_service;
        // Ptr to socket can be passed to forwarder object
        boost::shared_ptr<udp::socket> socket_ptr; 
       
        // Issue: Each read must have its own copy of remote endpoint and recv_buffer
        udp::endpoint remote_endpoint_;
        boost::array<char, 512> recv_buffer_;

        // To protect shared resource cout
        boost::mutex global_stream_lock;

        // Start receive packets
        void start_receive();

        // Handle received packets
        void handle_receive(const boost::system::error_code& error,
                                  size_t bytes_transferred);

        // Process received packets
        void process_packet(boost::array<char, 512>& packet, 
                            size_t packet_len, 
                            udp::endpoint remote_endpoint);

        // Handle request to send packet
        void handle_send(const boost::system::error_code& error, 
                               size_t bytes_transferred);
         
        // Replay request to DNS server
        boost::shared_ptr<screenDNS::forwarder> myForwarder;    
};

}

#endif /* __SERVER_H__ */

