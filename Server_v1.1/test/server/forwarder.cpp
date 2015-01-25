/**
 * @file   forwarder.cpp
 * @author dvv <dvv@avauntguard.com>
 * @brief  A forwarder to replay packet to DNS server
 * @test: dig @192.168.230.83 -p 10053 newsletterupdatecenterline.com
 */
#include "forwarder.h"

namespace screenDNS {
    
    // New Reader object when initialize forwarder
    forwarder::forwarder()
              : m_reader_IP(new Reader),
                m_reader_MALWARE(new Reader),
                m_reader_CHILDPORN(new Reader),
                m_reader_WHITELIST(new Reader)
    {}

    forwarder::~forwarder()
    {
        delete m_reader_IP;
        delete m_reader_MALWARE;
        delete m_reader_CHILDPORN;
        delete m_reader_WHITELIST;
    }
    
    void forwarder::setupReader(DBManager* manager_IP, DBManager* manager_MALWARE, 
                                DBManager* manager_CHILDPORN, 
                                DBManager* manager_WHITELIST)
    {   
        m_reader_IP->setup(manager_IP);
        m_reader_MALWARE->setup(manager_MALWARE);
        m_reader_CHILDPORN->setup(manager_CHILDPORN);
        m_reader_WHITELIST->setup(manager_WHITELIST);
    }

    void forwarder::print_debug_info(string debug_text)
    {
		#ifdef DBUG
		boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();		
		global_stream_lock.lock();
		cout << "Thread ID :: [" << boost::this_thread::get_id() << "]" << "\n";
		printf("DEBUG :: %s :: %s\n", to_simple_string(now).c_str(), debug_text.c_str());		
		global_stream_lock.unlock();
        #endif		
    }
    
    string forwarder::getTimeStamp() 
	{
        time_t rawtime;
        struct tm * timeinfo;
        char buffer [80];

        time ( &rawtime );
        timeinfo = localtime ( &rawtime );

        //Format "YYYY-MM-dd hh:mm :ss"
        strftime (buffer,80,"%Y-%m-%d %H:%M:%S",timeinfo);
        string time_stamp(buffer, strlen(buffer));

        return time_stamp;
    }
	
	// Function get all sub-strings of FQDN/CNAME
	vector<string> forwarder::getSubs(string Name)
	{
		vector<string> rs;
		rs.push_back(Name);
		
		string domain = "";
		vector<string> strs;
		boost::split(strs, Name, boost::is_any_of("."));
    
		// Normal case: "www.cnn.com"
		int offset = 0;
		vector<string>::iterator it;
		for ( it=strs.begin() ; it < strs.end()-1; it++ ) {
			offset += strlen((*it).c_str()) + 1;

			if ( offset < Name.size() ) {
				domain = Name.substr(offset);      						
				if ( it < strs.end()-2 ) {
					rs.push_back(domain); // Push back sub-domain
				} else if ( it == strs.end()-2 ){
					domain = "." + domain;
					rs.push_back(domain); // Push back sub-domain
				}
			}
		} // end for
		
		return rs;
	}
	
	// Function to verify vector in Whitelist CDB
	bool forwarder::verify_vector_InWhite(vector<string>& input) 
	{
		if (input.size() == 0) {
			#ifdef DBUG
				cout << "verify_vector_InWhite::Empty input\n";
			#endif
			return false; // Empty vector, nothing found
		}
		
		bool rs = false;
		
		vector<string>::iterator it;
		for(it = input.begin(); it < input.end(); it++) {
						
			// Convert domain to all-lower-case string
			std::string data = *it; 
			std::transform(data.begin(), data.end(), data.begin(), ::tolower);
			#ifdef DBUG
				cout << "verifying domain in White:_" << data << "_\n";
			#endif
			
			if (m_reader_WHITELIST->lookup(data)) {
				rs = true;
				break;
			}
		}
		
		return rs;
	}
	
	// Function to verify vector in CDB
	bool forwarder::verify_vector_InBlack(int tag, // What kind of lookup: CAI, Malice, etc...
										vector<string>& input, // All the strings need to be lookup in CDB
										string& blocked_domain, // Could be FQDN/sub-FQDN
										vector<string>& uni_resp // All the uni_resp strings found in CDB 
										) 
	{
		if (input.size() == 0) {
			#ifdef DBUG
				cout << "verify_vector_InBlack::Empty input\n";
			#endif
			return false; // Empty vector, nothing found
		}
		
		bool rs = false;
		
		vector<string>::iterator it;
		for(it = input.begin(); it < input.end(); it++) {
			#ifdef DBUG
				cout << "verifying domain in Black:_" << (*it) << "_\n";
			#endif
			if (verify_string_InBlack(tag, (*it), uni_resp)) {
				blocked_domain = (*it);
				rs = true;
				#ifdef DBUG
					cout << "Blocked_domain:_" << blocked_domain << "_\n";
				#endif
				break;
			}
		}
		
		return rs;
	}
	
	// Function to verify domain string in Blacklist CDB	
	bool forwarder::verify_string_InBlack(int tag, // What kind of lookup: CAI, Malice, etc...
							string domain, // String need to be lookup in CDB
							vector<string>& uni_resp // All the uni_resp strings found in CDB 
							) 
	{
		bool rs = false;
		// Convert domain to all-lower-case string
		std::string domain_lowercase = domain; 
		std::transform(domain_lowercase.begin(), domain_lowercase.end(), domain_lowercase.begin(), ::tolower);
		#ifdef DBUG
			cout << "verifying domain in Black:_" << domain_lowercase << "_\n";
		#endif
			
		// Port 10053
		if ( tag == atoi(Config::Instance()->get_port_protection_plus_1().c_str()) )
		{
		    // Incremental look up on CP+MW+RBL. If any of them T, return T
			vector<string> uni_resp_CHILDPORN;
			bool rs_CHILDPORN = m_reader_CHILDPORN->lookupUnifiedResponse(domain_lowercase,
													uni_resp_CHILDPORN); 
			if (rs_CHILDPORN && (uni_resp_CHILDPORN.size() > 0)) { 
				//Log uni_resp_CHILDPORN
				vector<string>::iterator it;
				for(it=uni_resp_CHILDPORN.begin();
					it!=uni_resp_CHILDPORN.end();++it)
					uni_resp.push_back(*it);
			}
			
			vector<string> uni_resp_MALWARE;
			bool rs_MALWARE = m_reader_MALWARE->lookupUnifiedResponse(domain_lowercase,
												uni_resp_MALWARE);                
			if (rs_MALWARE && (uni_resp_MALWARE.size() > 0)) {
				//Log uni_resp_MALWARE
				vector<string>::iterator it;
				for(it=uni_resp_MALWARE.begin();
					it!=uni_resp_MALWARE.end();++it)
					uni_resp.push_back(*it);
			}

			rs = rs_CHILDPORN || rs_MALWARE;
		}
		
		// Port 20053
		if ( tag == atoi(Config::Instance()->get_port_protection_plus_2().c_str()) )
		{
		    // Incremental look up on CP+MW. If any of them T, return T
			vector<string> uni_resp_CHILDPORN;
			bool rs_CHILDPORN = m_reader_CHILDPORN->lookupUnifiedResponse(domain_lowercase,
													uni_resp_CHILDPORN); 
			if (rs_CHILDPORN && (uni_resp_CHILDPORN.size() > 0)) { 
				//Log uni_resp_CHILDPORN
				vector<string>::iterator it;
				for(it=uni_resp_CHILDPORN.begin();
					it!=uni_resp_CHILDPORN.end();++it)
				    uni_resp.push_back(*it); 
			}
			
			vector<string> uni_resp_MALWARE;
			bool rs_MALWARE = m_reader_MALWARE->lookupUnifiedResponse(domain_lowercase,
												uni_resp_MALWARE);                
			if (rs_MALWARE && (uni_resp_MALWARE.size() > 0)) {
				//Log uni_resp_MALWARE
				vector<string>::iterator it;
				for(it=uni_resp_MALWARE.begin();
					it!=uni_resp_MALWARE.end();++it)
				    uni_resp.push_back(*it);
			}

			rs = rs_CHILDPORN || rs_MALWARE;
		}
		
		// Port 30053
		if ( tag == atoi(Config::Instance()->get_port_protection_plus_3().c_str()) )
		{
		    // Look up only CP. If any of them T, return T
			vector<string> uni_resp_CHILDPORN;
			rs = m_reader_CHILDPORN->lookupUnifiedResponse(domain_lowercase,
														   uni_resp_CHILDPORN);
			if (rs && (uni_resp_CHILDPORN.size() > 0)) { 
				//Log uni_resp_CHILDPORN
				vector<string>::iterator it;
				for(it=uni_resp_CHILDPORN.begin();
					it!=uni_resp_CHILDPORN.end();++it)
				    uni_resp.push_back(*it); 
			}
		}
		
		// Check IP
		if ( tag == 0 )
		{
			// Look up only CP. If any of them T, return T
			vector<string> uni_resp_IP;
			bool rs_IP = m_reader_IP->lookupUnifiedResponse(domain_lowercase,
														   uni_resp_IP);
			if (rs_IP && (uni_resp_IP.size() > 0)) { 
				//Log uni_resp_CHILDPORN
				vector<string>::iterator it;
				for(it=uni_resp_IP.begin();
					it!=uni_resp_IP.end();++it)
				    uni_resp.push_back(*it); 
			}
			
			rs = rs || rs_IP;
		}
		
		return rs;
	}

    // Function to verify DNS request. Return: True = Blocked, False: Not Blocked
	bool forwarder::verify_request(bool* p_ok,
						int tag, unsigned char* buf, size_t buf_len, // Input: Tag_port, packet, packet_length
						string& FQDN, // FQDN from Question[] section of Request packet
						string& blocked_domain, // Could FQDN/sub-FQDN
						vector<string>& uni_resp // All the uni_resp strings corresponding to blocked_domain 
						) 
	{
		bool rs = false;
		
		// Parse packet to get FQDN
		bool ok = false;
        struct DNS_Packet* packet = DNS_parse_packet(buf, buf_len, ok);
        *p_ok = ok; 
        if (!ok) {
            DNS_free_packet(packet);  
            return false; // Malformed DNS request
        }
		
		// Get FQDN
		FQDN = getFQDN(packet);
		DNS_free_packet(packet);
				
		// Check FQDN
		vector<string> subFQDN = getSubs(FQDN);
		
		rs = verify_vector_InWhite(subFQDN);
				
		// If FQDN/sub-FQDN not in Whitelist then check Blacklist
		if (!rs) {
			#ifdef DBUG
				cout << "FQDN/sub-FQDN is Not WhiteList\n";
			#endif
			rs = verify_vector_InBlack(tag, // What kind of lookup: CAI, Malice, etc...
									   subFQDN, // All the strings need to be lookup in CDB
									   blocked_domain, // Could be FQDN/sub-FQDN
									   uni_resp // All the uni_resp strings found in CDB 
									   );
		} else {
			#ifdef DBUG
				cout << "FQDN/sub-FQDN is WhiteList\n";
			#endif
			rs = false; // False: Not Blocked
		}
		
		// 
		return rs;
	}
	
	// Function to verify DNS response
	bool forwarder::verify_response(bool* p_ok,
						 bool* p_CNAMEInWhiteList, // Reserved to use for the auto-whitelist feature
						 bool* p_IPInBlackList,	// To flag if IP is blocked
						 int tag, unsigned char* buf, size_t buf_len,
						 string IP, // IP from normal response
						 vector<string>& IPs, // IPs need for logging
						 string& blocked_domain, // Could be blocked_IP or blocked_sub_CNAME
						 vector<string>& uni_resp // All the uni_resp strings corresponding to blocked_domain
						 )
	{
		bool inWhiteList = false;
		bool inBlackList = false;
		
		// Parse packet to get FQDN
		bool ok = false;
        struct DNS_Packet* packet = DNS_parse_packet(buf, buf_len, ok);
        *p_ok = ok; 
        if (!ok) {
            DNS_free_packet(packet);  
            return false; // Malformed DNS request
        }
		
		// Get CNAMEs
		vector<string> CNames = getCNames(packet);
		// Get last IP
		IP = getIP(packet);
		// Get IPs
		IPs = getIPs(packet);
		DNS_free_packet(packet);
		
		// Combine all CNAMEs/sub-CNAMEs into input vector
		vector<string> input;
		if (CNames.size() > 0) {
			vector<string>::iterator it_CNames;
			for(it_CNames = CNames.end()-1; it_CNames >= CNames.begin(); it_CNames--) {
				#ifdef DBUG
					cout << "Vector size = _" << CNames.size() << "_\n";
					cout << "One CNAME:_" << *it_CNames << "_\n";
				#endif
				string CName = *it_CNames; // crash because iteractor points to JUNK
				vector<string> sub_CName = getSubs(CName);
				if (sub_CName.size() > 0) {
					vector<string>::iterator it_sub_CName;
					for(it_sub_CName = sub_CName.end()-1; it_sub_CName >= sub_CName.begin(); it_sub_CName--) {
						input.push_back(*it_sub_CName);
					}
				} // end if (sub_CName.size() > 0)
			} // end for(it_CNames = CNames.end()-1; it_CNames >= CNames.begin(); it_CNames--)
		} // end if (CNames.size() > 0)
				
		inWhiteList = verify_vector_InWhite(input);
		// If CNAMEs/sub-CNAMEs not in Whitelist then check Blacklist
		if (!inWhiteList) {
			#ifdef DBUG
				cout << "!p_CNAMEInWhiteList\n";
			#endif
			inBlackList = verify_vector_InBlack(tag, // What kind of lookup: CAI, Malice, etc...
									   input, // All the strings need to be lookup in CDB
									   blocked_domain, // Could be blocked_sub_CNAME
									   uni_resp // All the uni_resp strings found in CDB 
									   ); 
		} else {
			#ifdef DBUG
				cout << "p_CNAMEInWhiteList\n";
			#endif
			*p_CNAMEInWhiteList = true;
		}
		
		// If not yet blocked, check all IPs
		if (!inWhiteList && !inBlackList) {
			inWhiteList = verify_vector_InWhite(IPs);
			// If IPs not in Whitelist then check Blacklist
			if (!inWhiteList) {
				inBlackList = verify_vector_InBlack(tag, // What kind of lookup: CAI, Malice, etc...
										   IPs, // All the strings need to be lookup in CDB
										   blocked_domain, // Could be blocked_IP
										   uni_resp // All the uni_resp strings found in CDB 
										   );
				// Check if blacklisted IP?						   
				if (!inBlackList)
					inBlackList = verify_vector_InBlack(0, // What kind of lookup: CAI, Malice, etc...
										   IPs, // All the strings need to be lookup in CDB
										   blocked_domain, // Could be blocked_IP
										   uni_resp // All the uni_resp strings found in CDB 
										   );
				*p_IPInBlackList = inBlackList;
			}
		}
		//	
		return inBlackList;
	}
	
	// 1st log: To get block reason
	void forwarder::doLoggingReason(string FQDN, string blocked_domain, vector<string> uni_resp)
	{
		if (uni_resp.size() > 0) {
			string time_stamp = getTimeStamp();
			vector<string>::iterator it;
			for ( it=uni_resp.end()-1; it >= uni_resp.begin(); it-- ) {
				// (FQDN, blocked_domain, IP="", (*it), zonename="", time_stamp)
				if ((*it).size() > 0)
					Logger::Instance()->log(FQDN, blocked_domain, "", (*it), "", time_stamp);
			} // end for
		} // end if (uni_resp.size() > 0)
	}
    
	// 2nd log: To do statistics task
	void forwarder::doLoggingStat(string FQDN, string blocked_domain, vector<string> IPs, vector<string> uni_resp)
	{	
		if ((IPs.size() > 0) && (uni_resp.size() > 0)) {
			string time_stamp = getTimeStamp();
			vector<string>::iterator it_IP;
			for ( it_IP = IPs.end()-1; it_IP >= IPs.begin(); it_IP-- ) {
				vector<string>::iterator it_uni_resp;
				for ( it_uni_resp = uni_resp.end()-1; it_uni_resp >= uni_resp.begin(); it_uni_resp-- ) {
					// (FQDN, blocked_domain, (*it_IP), (*it_uni_resp), zonename="", time_stamp)
					if (((*it_IP).size() > 0) && ((*it_uni_resp).size() > 0))
						Logger::Instance()->log(FQDN, blocked_domain, (*it_IP), (*it_uni_resp), "", time_stamp);
				}
			} // end for
		} // end if ((IPs.size() > 0) && (uni_resp.size() > 0))
	}
	
	// To get block reason for RBL
    void forwarder::doLoggingForRBL(string FQDN, string normal_IP, string IP_from_RBL, string zone)
    {
        string time_stamp = getTimeStamp();
		Logger::Instance()->log(FQDN, FQDN, normal_IP, IP_from_RBL, zone, time_stamp);
    }
	
	// To auto-whiteList
	void forwarder::doLoggingForWhiteList(string hostname, string country_code, string notes, string source)
	{
		Logger::Instance()->logWhiteList(hostname, country_code, notes, source);
	}
	
	// Relay block response
    void forwarder::sendBlockResponse(unsigned char* buffer, size_t packet_len,
                                      boost::shared_ptr<udp::socket> socket_ptr,
                                      udp::endpoint remote_endpoint)
    {
        // 1. Craft block response
        string TTL = Config::Instance()->get_TTL(); // string TTL("0.0.1.44");
        string IP = Config::Instance()->get_block_IP(); // string IP("202.123.2.6"); // CA 7B 02 06
        
		// Alloc memory for crafted DNS response
        size_t data_len = packet_len + 16;                 
        unsigned char* data = 
	    (unsigned char*)calloc(data_len, sizeof(unsigned char));
        
		// Craft block_resp data	    
        craft_DNS_response(buffer, packet_len, TTL, IP, data);
       
        // 2. Send block response back
        socket_ptr->async_send_to(
                    boost::asio::buffer(data, data_len),  
		    remote_endpoint,
		    boost::bind(&screenDNS::forwarder::handle_send_ng,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred,
                        data)
		);
    }
	
		// Relay packet to normal DNS server
    void forwarder::relay(
						boost::array<char, 512>& packet, size_t packet_len,
                        boost::array<char, 512>* recv_buf, 
                        size_t* recv_len) 
    {
		print_debug_info("Relay packet thread initiated ... ");
        *recv_len = 0;

        // Replay request to DNS server. Using native socket to send/receive with time out
	    boost::asio::io_service DNS_io_service;
	    udp::resolver resolver(DNS_io_service);
        
        // Nameserver IP
		string DNS_IP1 = Config::Instance()->get_DNS_IP1(); // string DNS_IP1 = "192.168.231.20";
		string DNS_IP2 = Config::Instance()->get_DNS_IP2(); // string DNS_IP2 = "192.168.231.23";	

		udp::resolver::query query1(udp::v4(), DNS_IP1, "53");
		udp::resolver::query query2(udp::v4(), DNS_IP2, "53");

		udp::endpoint receiver_endpoint1 = *resolver.resolve(query1);
		udp::endpoint receiver_endpoint2 = *resolver.resolve(query2);

		udp::socket DNS_socket(DNS_io_service);
		DNS_socket.open(udp::v4()); 
        
        try {

			DNS_socket.send_to(boost::asio::buffer(packet, packet_len),
                               receiver_endpoint1);  
            DNS_socket.send_to(boost::asio::buffer(packet, packet_len),
                               receiver_endpoint2);  

		} catch (exception& e) {
			cout << "forwarder::relay_send_to throws error: " << e.what() << endl;
			return;
		}

		// Set up a timed select call, so we can handle timeout cases.
		fd_set fileDescriptorSet;
		struct timeval timeStruct;

		// set the timeout to 2 seconds
		timeStruct.tv_sec = atoi(Config::Instance()->get_socket_to().c_str()); // 2;
		timeStruct.tv_usec = 0;
		FD_ZERO(&fileDescriptorSet);

		// We'll need to get the underlying native socket for this select call, in order
		// to add a simple timeout on the read:
		int nativeSocket = DNS_socket.native();

		FD_SET(nativeSocket,&fileDescriptorSet);
		select(nativeSocket+1,&fileDescriptorSet,NULL,NULL,&timeStruct);

		if(FD_ISSET(nativeSocket,&fileDescriptorSet)) {

			udp::endpoint sender_endpoint;
	    
            // Blocking receive from
            try {
                *recv_len = DNS_socket.receive_from(
													boost::asio::buffer(*recv_buf, 512), sender_endpoint);
            } catch (exception& e)
			{
    	        cout << "forwarder::relay_receive_from throws error: " 
                     << e.what() << endl;
                return;
			}
	    
			// Close the socket
			DNS_socket.close();
		}
    }
	
	// Replay packets after checking on White & Black lists
    void forwarder::relay_back(
								// input
								boost::array<char, 512>& packet, size_t packet_len,
								// let us know from which port the packet come from 
								int tag,
								// This function will send back the response itself after processing request
								boost::shared_ptr<udp::socket> socket_ptr, 
								udp::endpoint remote_endpoint) 
    {
        print_debug_info("Relay_back thread initiated ... ");
        
        bool ok = false; // Verify if DNS packet is malformed?
		string FQDN = "";
		string blocked_domain = "";
		vector<string> uni_resp;
        
		// 0: Convert from packet to buffer
        unsigned char buffer[packet_len];
        for (int i=0; i<packet_len; i++)
            buffer[i] = packet[i];

		bool blockedReq = verify_request(&ok,
								tag, buffer, packet_len, // Input: Tag_port, packet, packet_length
								FQDN, // FQDN from Question[] section of Request packet
								blocked_domain, // Could be FQDN/sub-FQDN
								uni_resp // All the uni_resp strings corresponding to blocked_domain 
								);
        if (!ok)
            return; // Malformed DNS request

        if (blockedReq) {
			sendBlockResponse(buffer, packet_len,
                              socket_ptr, remote_endpoint);
			// Log reason
			doLoggingReason(FQDN, blocked_domain, uni_resp);
        }

        // 1: Send the request out
        int recv_len = 0;
        boost::array<char, 512> recv_buf;
        // Replay request to DNS server. Using native socket to send/receive with time out
	    boost::asio::io_service DNS_io_service;
	    udp::resolver resolver(DNS_io_service);
        
        // Nameserver IP
	    string DNS_IP1 = Config::Instance()->get_DNS_IP1(); // string DNS_IP1 = "192.168.231.20";
	    string DNS_IP2 = Config::Instance()->get_DNS_IP2(); // string DNS_IP2 = "192.168.231.23";
	
	    udp::resolver::query query1(udp::v4(), DNS_IP1, "53");
	    udp::resolver::query query2(udp::v4(), DNS_IP2, "53");
	    udp::endpoint receiver_endpoint1 = *resolver.resolve(query1);
	    udp::endpoint receiver_endpoint2 = *resolver.resolve(query2);
	    udp::socket DNS_socket(DNS_io_service);
	    DNS_socket.open(udp::v4()); 
        
        try {
	        DNS_socket.send_to(boost::asio::buffer(packet, packet_len),
                               receiver_endpoint1);  
            DNS_socket.send_to(boost::asio::buffer(packet, packet_len),
                               receiver_endpoint2);
	    } catch (exception& e) {
	        cout << "forwarder::relay_send_to throws error: " 
                 << e.what() << endl;
	        return;
	    }

	    // Set up a timed select call, so we can handle timeout cases.
	    fd_set fileDescriptorSet;
	    struct timeval timeStruct;

	    // set the timeout to 2 seconds
	    timeStruct.tv_sec = atoi(Config::Instance()->get_socket_to().c_str()); // 2
	    timeStruct.tv_usec = 0;
	    FD_ZERO(&fileDescriptorSet);

	    // We'll need to get the underlying native socket for this select call, in order
	    // to add a simple timeout on the read:
	    int nativeSocket = DNS_socket.native();

	    FD_SET(nativeSocket,&fileDescriptorSet);
	    select(nativeSocket+1,&fileDescriptorSet,NULL,NULL,&timeStruct);

	    if(FD_ISSET(nativeSocket,&fileDescriptorSet)) {

	        udp::endpoint sender_endpoint;
	    
            // Blocking receive from
            try {
                recv_len = DNS_socket.receive_from(
	    	    boost::asio::buffer(recv_buf, 512), sender_endpoint);
            } catch (exception& e)
  	        {
    	        cout << "forwarder::relay_receive_from throws error: " << e.what() << endl;
                return;
  	        }
	    
	        // Close the socket
	        DNS_socket.close();
	    }

		if (recv_len > 0) {
			unsigned char resp_buf[recv_len];
            // Convert from packet to buffer
            for (int i=0; i<recv_len; i++)
				resp_buf[i] = recv_buf[i];
			
			bool CNAMEInWhiteList = false;
			bool IPInBlackList = false;
			string IP = "";
			string response_blocked_domain = "";
			vector<string> response_uni_resp;
			vector<string> IPs;
			
			bool blockedResp = verify_response(&ok,
									 &CNAMEInWhiteList, // Reserved to use for the auto-whitelist feature
									 &IPInBlackList,	// To flag if IP is blocked
									 tag, resp_buf, recv_len,
									 IP,
									 IPs, // IPs need for logging
									 response_blocked_domain, // Could be blocked_IP or blocked_sub_CNAME
									 response_uni_resp // All the uni_resp strings corresponding to blocked_domain
									 );
			
			if (!blockedResp) {
				if (!blockedReq) {
					// Send back normal response
					socket_ptr->async_send_to(boost::asio::buffer(recv_buf, recv_len),  
		                                  remote_endpoint,
		                                  boost::bind(&screenDNS::forwarder::handle_send,
			                              this,
			                              boost::asio::placeholders::error,
			                              boost::asio::placeholders::bytes_transferred));
				}
			} else {
				// Send back block response
				sendBlockResponse(buffer, packet_len,
                                  socket_ptr, remote_endpoint);
				
				// Log for statistics purpose
				if (IPInBlackList) {
					vector<string> blockedIP;
					blockedIP.push_back(response_blocked_domain);
					doLoggingStat(FQDN, response_blocked_domain, blockedIP, response_uni_resp);
				} else {
					doLoggingStat(FQDN, response_blocked_domain, IPs, response_uni_resp);
				} // end if (IPInBlackList)	
			} // end if (!blockedResp)
			
			#ifdef DBUG
				if (CNAMEInWhiteList)
					cout << "CNAMEInWhiteList\n";
				if (blockedReq)
					cout << "blockedReq\n";
			#endif
			
			// Log for auto-whiteList
			if (blockedReq && CNAMEInWhiteList)
				doLoggingForWhiteList(FQDN, "MU", "cname whitelist", "screenDNS");
				
		} // end if (recv_len > 0)		
    }
	
	// @ToDo: REFACTOR IT
    // Algo: void, passing (endpoint) so the code can resend the packet itself: 
    // Add 1 worker thread to query DNS server and get response
    // Send back DNS block_response instantly in monitor_thread
    void forwarder::relay_to_RBL(
                                int tag,
    				            boost::array<char, 512>& packet, 
                                size_t packet_len,
                           	    boost::shared_ptr<udp::socket> socket_ptr, 
				                udp::endpoint remote_endpoint)
    {
		print_debug_info("Relay_to_RBL thread initiated ... ");
        
        bool ok = false; // Verify if DNS packet is malformed?
		string FQDN = "";
		string blocked_domain = "";
		vector<string> uni_resp;
        
		// 0: Convert from packet to buffer
        unsigned char buffer[packet_len];
        for (int i=0; i<packet_len; i++)
            buffer[i] = packet[i];

		bool blockedReq = verify_request(&ok,
								tag, buffer, packet_len, // Input: Tag_port, packet, packet_length
								FQDN, // FQDN from Question[] section of Request packet
								blocked_domain, // Could be FQDN/sub-FQDN
								uni_resp // All the uni_resp strings corresponding to blocked_domain 
								);
        if (!ok)
            return; // Malformed DNS request

        if (blockedReq) {
            sendBlockResponse(buffer, packet_len,
                              socket_ptr, remote_endpoint);
			// Log reason
			doLoggingReason(FQDN, blocked_domain, uni_resp);
        }		
        
        // 1: Run workers & monitor threads to query RBLs    
        vector<string> strs;
		string list_RBLs = Config::Instance()->get_list_RBLs();
        boost::split(strs, list_RBLs, boost::is_any_of(";"));
		vector<string>::iterator it;
		
		// Reset global variables
		flag = false;
		zone = "";
		IP_from_RBL = "";
        sent_block_resp = false;
		completed_rbl_queries = 0;
        size_t num_workers = strs.size();
		
        boost::asio::io_service work_io_service;
	    boost::thread_group threads;
        size_t thread_group_size = Config::Instance()->get_num_RBLs() + 2; // 3 RBLs + 1 monitor + 1 normal query
		
        // Post DNS query service
        size_t recv_len;
        boost::array<char, 512> recv_buf;
 	    work_io_service.post(boost::bind(&screenDNS::forwarder::relay,
                                this,
                                packet, packet_len,
                                &recv_buf,
                                &recv_len));

        // Post monitor services
        work_io_service.post(boost::bind(&screenDNS::forwarder::monitor,
                                this,
                                // Wait for 2s
                                atoi(Config::Instance()->get_condition_var_to().c_str()),// 2000,
								num_workers,
                                buffer, packet_len,
                                socket_ptr, remote_endpoint));

        // Post work services
		for ( it=strs.begin() ; it < strs.end(); it++ ) {
		    string RBL_suffix = *it;
			work_io_service.post(boost::bind(&screenDNS::forwarder::find_fact,
                                 this,
                                 // Passing packet and packet_len 
                                 packet, packet_len,
								 RBL_suffix));
		}
		
        // Schedule thread
        for (size_t i = 0; i < thread_group_size; ++i)
   	        threads.create_thread(boost::bind(&boost::asio::io_service::run,
                                              &work_io_service));
 
	    work_io_service.reset();
	    work_io_service.run();
        threads.join_all();
        
        print_debug_info("forwarder:wait_condition_replay:All threads jointed");
		
		string IP = ""; // IP from normal response
		bool blockedResp = false;
		bool CNAMEInWhiteList = false;
		bool IPInBlackList = false;
		string response_blocked_domain = "";
		vector<string> response_uni_resp;
		vector<string> IPs;
		
		// Analyze normal response
        if (recv_len > 0) {
            unsigned char resp_buf[recv_len];
            // Convert from packet to buffer
            for (int i=0; i<recv_len; i++)
				resp_buf[i] = recv_buf[i];		
			
			blockedResp = verify_response(&ok,
									 &CNAMEInWhiteList, // Reserved to use for the auto-whitelist feature
									 &IPInBlackList,	// To flag if IP is blocked
									 tag, resp_buf, recv_len,
									 IP, // IP from normal response
									 IPs, // IPs need for logging
									 response_blocked_domain, // Could be blocked_IP or blocked_sub_CNAME
									 response_uni_resp // All the uni_resp strings corresponding to blocked_domain
									 );			
		}
		
        if (flag) {
            print_debug_info("forwarder:wait_condition_replay:Flag is TRUE");
			
            // Fall back: If the monitor thread did not send block response before
            if (!sent_block_resp) {
			    print_debug_info("forwarder:wait_condition_replay:Fall-back");
                sendBlockResponse(buffer, packet_len,
                                  socket_ptr, remote_endpoint);
            }
			
			// Log answer from RBL source if we got any
			if (zone.size()!=0)
				doLoggingForRBL(FQDN, IP, IP_from_RBL, zone);
        } else {
            print_debug_info("forwarder:wait_condition_replay:Flag is FALSE");
            
			if (!blockedResp) {
				if (!blockedReq) {
					// Send back normal response
					socket_ptr->async_send_to(boost::asio::buffer(recv_buf, recv_len),  
		                                  remote_endpoint,
		                                  boost::bind(&screenDNS::forwarder::handle_send,
			                              this,
			                              boost::asio::placeholders::error,
			                              boost::asio::placeholders::bytes_transferred));
				}
			} else {
				// Send back block response
				sendBlockResponse(buffer, packet_len,
                                  socket_ptr, remote_endpoint);
				
				// Log for statistics purpose
				if (IPInBlackList) {
					vector<string> blockedIP;
					blockedIP.push_back(response_blocked_domain);
					doLoggingStat(FQDN, response_blocked_domain, blockedIP, response_uni_resp);
				} else {
					doLoggingStat(FQDN, response_blocked_domain, IPs, response_uni_resp);
				} // end if (IPInBlackList)
			} // end if (!blockedResp)
			
			// Log for auto-whiteList
			if (blockedReq && CNAMEInWhiteList)
				doLoggingForWhiteList(FQDN, "MU", "cname whitelist", "screenDNS");
					
		} // end if(flag)	
    }
    
    void forwarder::monitor(int timeout,
	                        size_t num_workers,
                            unsigned char* buffer, size_t packet_len,
		                    boost::shared_ptr<udp::socket> socket_ptr, 
                            udp::endpoint remote_endpoint) 
    {
        print_debug_info("Monitor thread initiated ... ");

        // mutex_lock while(!flag) {cond_wait_with_timeout} mutex_unlock
        boost::mutex::scoped_lock lock(mut);

        // Loop until timeout or flag = T. Some other thread may reset flag to T before
        bool done = false;

        // CRITICAL CONDITION to continue monitoring loop
        while(!done && !flag && completed_rbl_queries < num_workers) {
            //cond.timed_wait: Not good yet as not wait for lock but wait till time out
	        if (!cond.timed_wait(lock, boost::posix_time::milliseconds(timeout)))
	        {
		        print_debug_info("forwarder:monitor:Time out"); 
		        done = true;
	        } else {
		        print_debug_info("forwarder:monitor:Notified");
	        }
        }
        
        // If blocked, async/send_to block response to remote_endpoint
  	    if (flag) {
            sendBlockResponse(buffer, packet_len,
                              socket_ptr, remote_endpoint);
            sent_block_resp = true;
	    }
        
        print_debug_info("forwarder:monitor:Terminated");
    }
    
    // Dont do any print out here or deadlock
    void forwarder::find_fact(boost::array<char, 512>& packet, 
                              size_t packet_len, 
                              string suffix)
    {

        // Step 1: Prod: Replay the packet and read_with_timeout response
        unsigned char buffer[packet_len];
        // Convert from packet to buffer
        for (int i=0; i<packet_len; i++)
            buffer[i] = packet[i];

        size_t query_len = 0;
        unsigned char* craft_query = 
            (unsigned char*)calloc(512, sizeof(unsigned char));
	
	    // In: buffer, packet_len, suffix. Out: craft_querry, query_len	    
        craft_DNS_query(buffer, packet_len, suffix, craft_query, query_len);
        
        // Convert packet
        boost::array<char, 512> query_packet;
        for (int i=0; i<query_len; i++)
            query_packet[i] = craft_query[i];

        // Relay packet
        size_t recv_len;
    	boost::array<char, 512> recv_buf;
    	relay(query_packet, query_len, &recv_buf, &recv_len);
        
        // Analyze recv_buf 
        bool blocked = false;
        string resolvedIP = "";
        if (recv_len > 0)
        {			
			// Convert
			unsigned char DNS_resp_packet[recv_len];
			for (int i=0; i<recv_len; i++)
                DNS_resp_packet[i] = recv_buf[i];
			
            // Parse packet 
            bool ok = true; // To know if response packet if malformatted?
	        struct DNS_Packet* resp_packet = DNS_parse_packet(DNS_resp_packet,
                                                              recv_len, ok);  
            
            // Decide if this is a block response if get AT LEAST 1 ANSWER
            if (ok && resp_packet->ancount)
	        {
                resolvedIP = getIP(resp_packet);
		        blocked = true;
	        }
	    
            DNS_free_packet(resp_packet);	
        }
		
        // Step 2: Lock mutex and update flag.
        //         If flag==T, notify monitor_thread. Unlock mutex
        // Notify monitor thread if blocked or number of compeled_rbl_queries reached
        // So we can terminate monitor thread earlier
        boost::mutex::scoped_lock lock(mut);
		
        if(!flag && completed_rbl_queries < Config::Instance()->get_num_RBLs())
	    {

	        if(blocked) {
                flag = true;
                zone = suffix;
                IP_from_RBL = resolvedIP; 
            } else {
		        completed_rbl_queries++;
            }

	        // If we do not notify monitor thread when all our RBL threads have executed
	        // then the monitor thread will run for 2 seconds before sending response O_o
	        if(flag || completed_rbl_queries >= Config::Instance()->get_num_RBLs())
	            cond.notify_one();			
	    }

        // Free resource
        free(craft_query);     
    }

    // Handle request to send packet. Standard version
    void forwarder::handle_send(const boost::system::error_code& error,
                         size_t bytes_transferred)
    {}

    // Handle request to send packet. Extended version. 
    // Free allocated buffer for packet after sending
    void forwarder::handle_send_ng(const boost::system::error_code& error, 
                        size_t bytes_transferred,
                        unsigned char* pData)
    {
        free(pData);
        handle_send(error, bytes_transferred);
    }  
}

