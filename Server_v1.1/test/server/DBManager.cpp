#include "DBManager.h"
// http://stackoverflow.com/questions/3963771/example-of-using-scoped-try-shared-lock-and-upgrade-lock-in-boost

namespace screenDNS {
    
    DBManager::DBManager()
              :initialized(false) {}

    void DBManager::setupDB(string cdbDirPath, DB_t dbType) {
        m_cdbDirPath = cdbDirPath;
        m_dbType = dbType;
    }
    
    // Callable for now from a single Monitor thread
    void DBManager::writeDB(string log_content) {
    
        string dbFileName = "";
        cout << "Get notified. log_content is " << log_content << endl;  
        
        // REFACTOR: "/usr/home/dvv/" + "Malware" + "1/2.cdb"
        string suffix;
        if (log_content == Config::Instance()->get_log_content_1()) // "F1 is in use"
            suffix = "1.cdb";
        else if (log_content == Config::Instance()->get_log_content_2()) // "F2 is in use"
            suffix = "2.cdb";
 
        switch(m_dbType) {
			case IP:
					dbFileName = m_cdbDirPath + "/" + Config::Instance()->get_dbFileName_ip() + suffix; // "ip"
				break;
			case MALWARE:
					dbFileName = m_cdbDirPath + "/" + Config::Instance()->get_dbFileName_malware() + suffix; // "malware"
				break;
			case CHILDPORN:
					dbFileName = m_cdbDirPath + "/" + Config::Instance()->get_dbFileName_childporn() + suffix; // "childporn"
				break;
				case WHITELIST:
					dbFileName = m_cdbDirPath + "/" + Config::Instance()->get_dbFileName_whitelist() + suffix; // "whitelist"
				break;
			default:
	        break; 
        }
        // END REFACTOR
		
	    int fd = open(dbFileName.c_str(), O_RDONLY);

        boost::upgrade_lock<boost::shared_mutex> write_lock(rw_mutex,
                                                            boost::try_to_lock); 
        boost::upgrade_to_unique_lock<boost::shared_mutex> w_unique_lock(write_lock);

		if (w_unique_lock) {	
				
			// Populate the DB in memory from physical file
			if (fd > 0) {
					// Free old cdb if it was allocated
				if (initialized) {
					cdb_free(&c);
				}
				
				cdb_init(&c, fd);
				close(fd);
				initialized = true;
				cout << "Database is initialized\n" << endl;			
			}
			cout << "Writer OK to acquire w_unique_lock" << endl;
		} else {
			cout << "Writer failed to acquire w_unique_lock" << endl;
		}
    }

    bool DBManager::readDB(struct cdb &cdb) {
	
        boost::shared_lock<boost::shared_mutex> r_lock(rw_mutex, boost::try_to_lock);
			
		if (r_lock && initialized){
			// We have obtained a shared lock
			cdb = c;
			return true;
		} else {
		    #ifdef DBUG
			if(!r_lock) 
				cout << "Reader failed to acquire lock\n" << endl;
			else
				cout << "Reader OK to acquire lock\n" << endl;

			if (!initialized)
				cout << "Database has not been initialized\n" << endl;
			else
				cout << "Database has been initialized\n" << endl;
		    #endif
			
			return false;
		}
			
	}
}
