#ifndef __DBMANAGER_H__
#define __DBMANAGER__

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

// For posix::time
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

// For cdb
#include <cdb.h>
#include <fcntl.h>

#include "Config.h"

using namespace std;

// +DOMAIN ??
typedef enum {
    IP = 0,
    MALWARE = 1,
    CHILDPORN = 2,
    WHITELIST = 3
} DB_t;

namespace screenDNS {

    // Implement 1_Writer, N_Readers using shared_mutex. 
    // See later if we need to implement Singleton
    class DBManager {
    public:
        // Constructor
        DBManager();

        void setupDB(string cdbDirPath, DB_t dbType);

        // log_content will give hint from which physical file we populate pointer to CDB in memory
        void writeDB(string log_content);
	// Pass structure using a structure reference
        bool readDB(struct cdb &cdb);

        // Reader and Monitor classes need to know database type 
        DB_t getDBType() { return m_dbType; };
        string getcdbDirPath() { return m_cdbDirPath; }; 

    private:

        // Construct full path to cdb file, e.g.: "/usr/home/dvv/" + "Malware" + "1/2" + ".cdb"
        string m_cdbDirPath; // e.g. "/usr/home/dvv/"
        DB_t m_dbType;

        // Share mutex to synchronize access Read/Write to DB
        mutable boost::shared_mutex rw_mutex;
         
        // DB here is conceptually modelled as a string, which is supposed to be the content of LOG
        struct cdb c; // Pointer to content of DB in memory
	    bool initialized; // To signal if c is already populated or not
    };

}

#endif /* __DBMANAGER_H__ */
