#ifndef __READER_H__
#define __READER__

#ifndef MAIN
#include "DBManager.h"
#endif

#include <iostream>
#include <fstream>

// For posix::time
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

using namespace std;
using namespace screenDNS;

namespace screenDNS {

    class Reader {
    public:
        // Constructor
        Reader(int id, DBManager* db_manager);
        
        // Callable from another thread
        void operator () (); 

    private:
        DBManager* m_db_manager;
        int m_id;
    };
}

#endif /* __READER_H__ */
