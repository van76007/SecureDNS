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

#ifndef NAMESERVER_DNS_H
#define NAMESERVER_DNS_H

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
#include <boost/thread/thread.hpp>
#include <time.h>
#include <fcntl.h>
#include <map>
#include <mysql/mysql.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
//#include "ExceptionHandler.h"
#include <stddef.h>
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

  class DNSNameserver : public DNSEntity
  {
  private:
    /**s
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
    DNSNameserver( const DNSNameserver& ){};
/*DNSNameserver
:Base()
, (const DNSNameserver&)
{}
*/
    int ListenUDP( int main_socket, 
		    socklen_t socket_length, 
		    struct sockaddr_in server_info, 
		    std::vector <int> statusParser);

	
  public:
DNSNameserver();
~DNSNameserver();
    /**
     * @brief Get an instance of the DNSNameserver
     *
     *
     * @return DNSNameserver a pointer to the DNSNameserver object.
     * @see DNSNameserver()
     */
    static DNSNameserver * GetInstance();
    int StartNameServer(	int local_port, 
				std::string IPAddress,  
				std::vector <int> statusParser );
					
    //std::string findName(int sizeQuery, unsigned char* CompleteQuery);
  private:
    static DNSNameserver* mpsInstance; ///< Pointer to the singleton OTFEStore
    /*	struct sockaddr_in local_infoc;
	unsigned int socklen;
	int Incoming_Socket;
    */	struct hostent *primary_nameserver;
    /*int QueryMainNameserver(unsigned char *response_buffer, 
      int response_size, 
      unsigned char* request_buffer, 
      int request_size, 
      std::string IP_Address, 
      int remote_port, int main_socket, unsigned int socket_length, struct sockaddr_in server_info);
    */
	
    //	void writetolog(MYSQL* connectionDat, std::string domain, std::string client_ip);

    //void* QueryRBLNameserver(void* ptr);


  }; 
}; //namespace Namserver

#endif // NAMESERVER_DNS_H
