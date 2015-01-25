#include "Monitor.h"

namespace screenDNS {

    Monitor::Monitor(DBManager* db_manager) {
        m_db_manager = db_manager;
        m_dbType = db_manager->getDBType();
        m_cdbDirPath = db_manager->getcdbDirPath();
    }

    void Monitor::operator () () {
	
	    // For kqueue()
	    int f, kq, nev;
	    struct kevent change;
	    struct kevent event;

	    kq = kqueue();
	    if (kq == -1)
		perror("kqueue");

            // REFACTOR
        string dbLogFile;
        switch(m_dbType) {
			case IP:
				dbLogFile = m_cdbDirPath + "/" + Config::Instance()->get_dbLogFile_ip(); // "dblog_ip"
				break;
			case MALWARE:
				dbLogFile = m_cdbDirPath + "/" + Config::Instance()->get_dbLogFile_malware(); // "dblog_malware"
				break;
			case CHILDPORN:
				dbLogFile = m_cdbDirPath + "/" + Config::Instance()->get_dbLogFile_childporn(); // "dblog_childporn"
				break;
			case WHITELIST:
				dbLogFile = m_cdbDirPath + "/" + Config::Instance()->get_dbLogFile_whitelist(); // "dblog_whitelist"
				break;
			default:
				break; 
	    }

	    f = open(dbLogFile.c_str(), O_RDONLY);
	    if (f == -1)
			cout << "Error open file " << dbLogFile << endl;
	            
	    EV_SET(&change, f, EVFILT_VNODE,
		EV_ADD | EV_ENABLE | EV_ONESHOT,
		NOTE_DELETE | NOTE_EXTEND | NOTE_WRITE,
		0, 0);
		
	    // First run: Read DB log file content if there and update DB pointer
	    string line;
        ifstream myfile(dbLogFile.c_str());            

	    if (!myfile.fail()) {
            getline (myfile,line);
			m_db_manager->writeDB(line);
	    }
	    
	    // Monitoring the change in content of DB log file
	    while (true)
	    {
            //c++;
            try {
		        // Monitor content of DB log file
		        nev = kevent(kq, &change, 1, &event, 1, NULL);
		        if (nev == -1) {
		            cout << "kevent error" << endl;
		        }
		        else if (nev > 0) 
		        {
		            if (event.fflags & NOTE_DELETE) {
		                cout << "Non-existent file" << endl;
		                
		            }
			   
		            if (event.fflags & NOTE_EXTEND ||
		                event.fflags & NOTE_WRITE)
			        {
			            string line;
                        ifstream myfile(dbLogFile.c_str());
 
						if (!myfile.fail()) {
							getline (myfile,line);
							cout << "LOG modified. Content: " << line << endl;
							// Update DB
							m_db_manager->writeDB(line);
						}
					}
		        }
	            
				// Dont busy-pooling too much as we update LOG once/day
				//sleep(60); // Check every 1 min 
				
		        // Check if this thread is interrupted by main
		        boost::this_thread::interruption_point();
		    }
		    catch (const boost::thread_interrupted&)
		    {
		        cout << "Get thread interrupted. Exiting..." << endl;
		        // Clean it up
		        close(kq);
	   	        close(f);
		        break;
		    }    
	    }
    }
}
