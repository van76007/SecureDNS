#include "DBManager.h"
// http://stackoverflow.com/questions/3963771/example-of-using-scoped-try-shared-lock-and-upgrade-lock-in-boost

namespace screenDNS {
    
    DBManager::DBManager():initialized(false) {}
    
    // Callable for now from a single Monitor thread
    void DBManager::writeDB(std::string log_content) {
	
	    std::string dbFileName("");
		if (log_content == "F1 is in use") {
			// Load cdb from F1
			dbFileName = "/usr/home/dvv/F1.cdb";
		}
		else if (log_content == "F2 is in use") {
			// Load cdb from F2
			dbFileName = "/usr/home/dvv/F2.cdb";
		}
		
		int fd = open(dbFileName.c_str(), O_RDONLY);
        
        boost::upgrade_lock<boost::shared_mutex> write_lock(rw_mutex, boost::try_to_lock); 
        boost::upgrade_to_unique_lock<boost::shared_mutex> w_unique_lock(write_lock);

	    if (w_unique_lock) {
			
			boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
		    std::cout << "log_content is " << log_content << " since " << boost::posix_time::to_iso_extended_string(now) << std::endl;	
			
			// Populate the DB in memory from physical file
			if (fd > 0) {
			    if (initialized)
				    cdb_free(&c); // Free old cdb if it was allocated. Otherwise, leave it alone. CRASHED
				cdb_init(&c, fd);
				close(fd);
                initialized = true;				
			}
			std::cout << "Writer OK to acquire w_unique_lock" << std::endl;
	    } else {
		    std::cout << "Writer failed to acquire w_unique_lock" << std::endl;
		}
        
    }

	bool DBManager::readDB(struct cdb &cdb) {
	
        boost::shared_lock<boost::shared_mutex> r_lock(rw_mutex, boost::try_to_lock);
			
	    if (r_lock && initialized){
  	        // We have obtained a shared lock
			cdb = c;
			return true;
	    } else {
		    if(!r_lock) 
		        std::cout << "Reader failed to acquire lock\n" << std::endl;
			else
			    std::cout << "Reader OK to acquire lock\n" << std::endl;
			if (!initialized)
			    std::cout << "Database has not been initialized\n" << std::endl;
			else
			    std::cout << "Database has been initialized\n" << std::endl;
		    return false;
        }
		
	}
}
