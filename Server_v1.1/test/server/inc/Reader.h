#ifndef __READER_H__
#define __READER__

#ifndef MAIN
#include "DBManager.h"
#endif

#include <iostream>
#include <fstream>
#include <vector>

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
        Reader();
        
        void setup(DBManager* db_manager);
        
        // Just need to know if we find the key in DB or not
        bool lookup(string key);

        // Provide look-up service to forwarder class
        bool lookupValue(string key, string& value);
        
        // Provide look-up service to forwarder class. Given key. Return T/F and Vector of Unified
        bool lookupUnifiedResponse(string key, vector<string>& unified_resp);

    private:
        DBManager* m_db_manager;
        DB_t m_dbType;
    };
}

#endif /* __READER_H__ */
