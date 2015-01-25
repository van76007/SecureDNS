/**
 * @file
 * @author SNU <snu@csis.dk>
 * @author DCO <dco@csis.dk>	
 * @brief Header file for DNSNamserver
 *
 * @section Description
 * This file contains the definition of DNSNamserver
 * 
 *
 */

#ifndef NAMESERVER_DNSStructure_H
#define NAMESERVER_DNSStructure_H

#include <DNSEntity.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <iostream>
#include <arpa/inet.h>
#include <pthread.h>
#include <mysql/mysql.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>

extern pthread_mutex_t mutex;
#define DNSLENGTH 512
#define PRODUCTION

namespace Nameserver {
  /**
   * @brief Nameserver 
   *
   * The Nameserver is a singleton object that on first initialization
   * will probe and gather various information. This includes:
   * @section Purpose
   * To gather and expose various relevant information.
   *
   *
   * @section Limitations
   * None
   *
   */

  class DNSstructure : public DNSEntity
  {
  private:
    /**
     * @brief Constructor Private -> singleton
     * The constructor is called from getInstance.
     *
     * @section Pre-conditions
     * The Constructor has not been called before and no infomation
     * is available.
     * @section Post-conditions
     * The variables is initialized and are exposed.
     * Or An exception is thrown should some variables be unavailable
     *
     * @see getInstance
     */
	
    /**
     * @brief A destructor.
     * A more elaborate description of the destructor.
     */
   

    /* Disable copy constructor */
    DNSstructure( const DNSstructure& ){};
    //	void ListenUDP(int main_socket, unsigned int socket_length, struct sockaddr_in server_info);
	
	
  public:
  DNSstructure();
 ~DNSstructure();
    typedef struct SizeSocketPort{ 
      int DNSsize; 
      int Socketdes; 
      int PortNo;
      int MSocket;
    }SSPM;
    /**
     * @brief Get an instance of the DNSstructure
     *
     *
     * @return DNSstructure a pointer to the DNSstructure object.
     * @see DNSstructure()
     */
    static DNSstructure* GetInstance();
    //	void StartNameServer(int local_port, int IPAddress);
    void ListCNIP(const char* request_buffer, unsigned char* response_buffer, int bytecount, std::vector<std::string>& IPLi, std::string* RealHostname);
    int CreateQuery(  int MainQuerySize, 
		      std::string MainQuery_buffer, 
		      std::string IPNameServ,
		      int PortNameServ,
		      std::string hostname, 
		      std::string Origdomain,
		      int MainIncoming_Socket, 
		      struct sockaddr_in Mainlocal_info,
		      unsigned int Mainsocklen/*, int &Blvect*/);

    void QuickTestCNIP(int size_request, unsigned char* response_buffer, int bytecount, std::vector<std::string>& IPLi, std::string* RealHostname);


    int CreateBlockedResponse( std::string response_array, 
				int numberReceived,  
				int main_socket,
				struct sockaddr_in local_info,
				unsigned int socklen, 	
				std::string domainName);
 
						

						
  private:
    static DNSstructure* mpsInstance; //
    void ChangetoDnsNameFormat( char* dns, std::string host);
	
	
  }; 
}; //namespace Namserver


#endif // NAMESERVER_DNSstructure_H
