#include "Reader.h"

namespace screenDNS {

    Reader::Reader(int id, DBManager* db_manager) {
        m_db_manager = db_manager;
        m_id = id;
    }
	
    void Reader::operator () () {
        while (true)
		{
			try {

				sleep(3); // sleep 3s before each read
	 
				// Read the content of DB by invoking DBManager.readDB() method
				struct cdb c;
                if (!m_db_manager->readDB(c))
                    continue; // Failed to obtain pointer to DB in memory. Skip the rest of the code

                // Open a log file for this thread
				std::ostringstream oss;
				oss << m_id;
				std::string fileName = oss.str() + "_";
				boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
				fileName += boost::posix_time::to_iso_extended_string(now);
					
				ofstream myfile;
				myfile.open (fileName.c_str());
				
				myfile << m_id << " Pass cdb_init...\n";
				
				// Test hard-coded loading DB
				/*
				std::cout << m_id << " Start test...\n" << std::endl;
				std::string dbFileName = "/usr/home/dvv/F1.cdb";
				int fd = open(dbFileName.c_str(), O_RDONLY);
				struct cdb c;
				
				if (fd > 0) {
				    std::cout << m_id << " Before cdb_free...\n" << std::endl;
			        //cdb_free(&c); // Free old cdb if it was allocated. Otherwise, leave it alone. CRASHED
					std::cout << m_id << " Past cdb_free...\n" << std::endl;
			        cdb_init(&c, fd);
					std::cout << m_id << " Pass cdb_init...\n" << std::endl;
                    close(fd);
					std::cout << m_id << " Pass close...\n" << std::endl;	
			    }
				struct cdb *cdbp = &c; // cdb_datalen(&c) equiv. cdb_datalen(cdbp)
				std::cout << m_id << " End test...\n" << std::endl;
				*/
				// End test loading DB
				
				// Do the query on key. Return either: Found/Not_Found Value Or List_Of_Values. The list will be pushed back into a vector
				// Ref: http://www.corpit.ru/mjt/tinycdb.html
			    std::string key;
				char *val;
				unsigned vlen, vpos;
				
				// Simulate the test. With different URLs. Those URLs should exist on F1
				if(m_id == 1)
				    key = "4t6nhh.6600.org";
				else if(m_id == 2)
				    key = "4tag16ag100.com";
				else if(m_id == 3)
				    key = "4root.net";
                else
                    key = "3uxyctrlmiqeo.cn";				
				
				// Version 1: Check existence of key only. And return std::string format of value if need. OK
				if (cdb_find(&c, key.c_str(), key.size()) > 0) { // if search successeful
				    vpos = cdb_datapos(&c); // position of data in a file
				    vlen = cdb_datalen(&c); // length of data
					val = (char*)malloc(vlen * sizeof(char*)); // allocate memory
				    cdb_read(&c, val, vlen, vpos); // read the value into buffer
				    // handle the value
				    val[vlen] = '\0';
				    std::string str_val(val); // This will be the std::string format of value
					free(val);
					
					// Debug
					std::cout << m_id << " Found key " << key << std::endl;
					myfile << "Value_" << str_val << "_Key_" << key << "_Found by Thread_" << m_id << "\n";
				} else {
				    
					// Debug
				    std::cout << m_id << " Found no key " << key << std::endl;
					myfile << "Found no Key_" << key << "_by Thread_" << m_id << "\n";
				}
				
				// Version 2: Check ALL occurences of key. Push all the found values into a vector. OK. Need to check vector
				/*
				std::vector<std::string> unified_resp;
				struct cdb_find cdbf; // structure to hold current find position
				cdb_findinit(&cdbf, &c, key.c_str(), key.size()); // initialize search of key
				std::cout << m_id << " Searching for key " << key << "_" << std::endl;
				while(cdb_findnext(&cdbf) > 0) {
				    vpos = cdb_datapos(&c);
				    vlen = cdb_datalen(&c);
				    val = (char*)malloc(vlen * sizeof(char*)); // allocate memory
				    cdb_read(&c, val, vlen, vpos);
				    // handle the value
				    val[vlen] = '\0';
				    std::string str_val(val); // This will be the std::string format of value
					std::cout << m_id << " Found 1 value " << str_val << " for key " << key << std::endl;
					myfile << m_id << " Found 1 value " << str_val << " for key " << key << std::endl;
				    // Push it into an answer vector
					unified_resp.push_back(str_val);
					free(val);
				}
				std::cout << m_id << " End searching for key " << key << "_" << std::endl;
				*/
				
				myfile.close();
				// Check if this thread is interrupted by main
				boost::this_thread::interruption_point();
			}
			catch (const boost::thread_interrupted&)
			{
				std::cout << "Get signal thread interrupted. Exiting reader thread" << std::endl;
				break;
			}    
	    }
    }
}
