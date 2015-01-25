#ifndef __READER_H__
#define __READER__

#ifndef MAIN
#include "Logger.h"
#endif

#include <iostream>
#include <fstream>

#include <boost/thread.hpp>
// For posix::time
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

using namespace std;
using namespace screenDNS;

namespace screenDNS {

    class Worker {
    public:
        // Constructor
        Worker(int id);
        
        // Callable from another thread
        void operator () (); 

    private:
        int m_id;
    };
}

#endif /* __READER_H__ */
