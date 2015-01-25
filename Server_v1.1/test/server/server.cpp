/**
 * @file   server.cpp
 * @author dvv <dvv@avauntguard.com>
 * @brief  A relay DNS server using Boost Asio threadpool
 */
#include "server.h"

using boost::asio::ip::udp;

namespace screenDNS {

server::server(int port, size_t thread_pool_size,
               DBManager* manager_IP, DBManager* manager_MALWARE, 
               DBManager* manager_CHILDPORN, DBManager* manager_WHITELIST)
	: thread_pool_size_(thread_pool_size),
          socket_ptr( new udp::socket(sock_io_service, udp::endpoint(udp::v4(), port)) ),
          myForwarder(new forwarder)
{
    myForwarder->setupReader(manager_IP, manager_MALWARE, 
                             manager_CHILDPORN, manager_WHITELIST);

    m_port = port;

    // Bind UDP socket
    start_receive();
}

server::~server() {
    socket_ptr->close();
}   

void server::run() {
    
    cout << "server::run" << "\n";

    // Create some work to stop io_service_.run() function from exiting if it has nothing else to do
    boost::shared_ptr< boost::asio::io_service::work > work(
        new boost::asio::io_service::work( io_service_ )
    );

    // Create a pool of threads to run all of the io_services.
    vector<boost::shared_ptr<boost::thread> > threads;
    
    // Schedule socket
    boost::shared_ptr<boost::thread> thread(new boost::thread(
            boost::bind(&boost::asio::io_service::run, &sock_io_service)));
    threads.push_back(thread);
    
    for (size_t i = 0; i < thread_pool_size_; ++i)
    {
        // Catch exception like non-sudo binding to port < 1024 like in old version
		try {
		    boost::shared_ptr<boost::thread> thread(new boost::thread(
            boost::bind(&boost::asio::io_service::run, &io_service_)));
            threads.push_back(thread);
		} catch (exception& e)
	    {
		    cout << "exception: " << e.what() << "\n";
			cout << "Server dies" << "\n";
	    }
    }
     
     cout << "server::run: Waiting for threads jointed" << "\n";

     // Wait for all threads in the pool to exit.
     for (size_t i = 0; i < threads.size(); ++i)
         threads[i]->join();
   
     cout << "server::run: All threads jointed. Terminated" << "\n";
}

void server::start_receive() {
    #ifdef DBUG
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
    global_stream_lock.lock();
    cout << "server::start_receive at " << to_iso_extended_string(now) << "\n";
    global_stream_lock.unlock();
    #endif
	
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

void server::handle_receive(const boost::system::error_code& error, size_t bytes_transferred) {
    #ifdef DBUG
    global_stream_lock.lock();
    cout << "server::handle_receive: Receiving data len " << bytes_transferred << "\n";
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
    cout << "Post job at: "<< to_iso_extended_string(now)<< endl;
    global_stream_lock.unlock();
    #endif
	
    // io_service post work to thread
    io_service_.post(boost::bind(&screenDNS::server::process_packet,
                                 this, 
                                 recv_buffer_, bytes_transferred, remote_endpoint_));
    
    // ready to receive packets again
    start_receive();
}

// Should make new object
void server::process_packet(boost::array<char, 512>& packet, size_t packet_len, udp::endpoint remote_endpoint) {
    #ifdef DBUG
    global_stream_lock.lock();
    cout << "server::process packet len " << packet_len << " in thread[" << boost::this_thread::get_id() << "]" << "\n";
    global_stream_lock.unlock();
    #endif
	
    // Different actions depended on m_port. 
    // Should we pass m_port to Forwarder as well so it knows what to do with the packet?
    // Should we let Forwarder handles send_back packet as well?
    // Port 10053
	if ( m_port == atoi(Config::Instance()->get_port_protection_plus_1().c_str()) )
	{
	    // Relay to RBL also have to lookup DB, at least for the DNS_request
        myForwarder->relay_to_RBL(m_port, packet, packet_len, socket_ptr, remote_endpoint);
	}
	
	// Port 20053 or 30053
	if ( ( m_port == atoi(Config::Instance()->get_port_protection_plus_2().c_str()) )
       ||( m_port == atoi(Config::Instance()->get_port_protection_plus_3().c_str()) ) )
	{
	    myForwarder->relay_back(packet, packet_len, m_port, socket_ptr, remote_endpoint);
	}
}

// To be invoked after sent
void server::handle_send(const boost::system::error_code& error,
                         size_t bytes_transferred)
{
    #ifdef DBUG
	if (!error && bytes_transferred > 0)
    {
        global_stream_lock.lock();
        cout << "server::handle_send OK. bytes_transferred is " << bytes_transferred << "\n";
        global_stream_lock.unlock();
    } else {
        global_stream_lock.lock();
        cout << "server::handle_send ERROR. bytes_transferred is " << bytes_transferred << "\n";
        global_stream_lock.unlock();
    }
	#endif
}

}
