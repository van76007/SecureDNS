#include "Monitor.h"

namespace screenDNS {

    Monitor::Monitor(DBManager* db_manager) {
        m_db_manager = db_manager;
    }

    void Monitor::operator () () {
	
	    // For kqueue()
	    int f, kq, nev;
	    struct kevent change;
	    struct kevent event;

	    kq = kqueue();
	    if (kq == -1)
		perror("kqueue");

	    f = open("/usr/home/dvv/foo", O_RDONLY);
	    if (f == -1)
		perror("open");
	   
	    EV_SET(&change, f, EVFILT_VNODE,
		EV_ADD | EV_ENABLE | EV_ONESHOT,
		NOTE_DELETE | NOTE_EXTEND | NOTE_WRITE,
		0, 0);
		
		// First run: Read DB log file content if there and update DB pointer
		std::string line;
	    ifstream myfile("/usr/home/dvv/foo");
		if (!myfile.fail()) {
		    getline (myfile,line);
			std::cout << "First run LOG: " << line << std::endl;
			// Update DB
			m_db_manager->writeDB(line);
		}
	    
	    // Monitoring the change in content of DB log file "foo" in background. PASSIVE MODE
        int c=0;
	    while (true)
	    {
		    c++;
            try {
		        // Monitor content of DB log file
		        nev = kevent(kq, &change, 1, &event, 1, NULL);
		        if (nev == -1) {
		            std::cout << "kevent error" << std::endl;
		        }
		        else if (nev > 0) 
		        {
		            if (event.fflags & NOTE_DELETE) {
		                printf("File deleted\n");
		                //break; // stop monitoring if the DB log file no longer exist
						sleep(60); // Don't too much busy-pooling
		            }
			   
		            if (event.fflags & NOTE_EXTEND ||
		                event.fflags & NOTE_WRITE)
			        { 	   
		                printf("LOG modified at step %d\n", c);
			            
						// Read file content of DB log file
			            std::string line;
			            ifstream myfile("/usr/home/dvv/foo");
			            if (!myfile.fail()) {
							getline (myfile,line);
							std::cout << "First run LOG: " << line << std::endl;
							// Update DB
							m_db_manager->writeDB(line);
						}
			        }
		        }
	 
		        // Check if this thread is interrupted by main
		        boost::this_thread::interruption_point();
		    }
		    catch (const boost::thread_interrupted&)
		    {
		        std::cout << "Get Notification thread interrupted. Exiting thread" << std::endl;
		        // Clean it up
		        close(kq);
	   	        close(f);
		        break;
		    }    
	    }
    }
}
