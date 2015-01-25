#include "Worker.h"

using namespace screenDNS;

namespace screenDNS {

    Worker::Worker(int id) {
        m_id = id;
    }
	
    void Worker::operator () () {
        while (true)
		{
			try {

				sleep(3); // sleep 3s before each read
				
				std::ostringstream oss;
				oss << m_id;
				std::string msg = "Thread_" + oss.str();
				msg += "_write2log_at_";
				
				boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
				msg += boost::posix_time::to_iso_extended_string(now);
				
				Logger::Instance()->writeToLogFile(msg);
	            
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
