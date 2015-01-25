#include "Reader.h"

namespace screenDNS {

    Reader::Reader() {}

    void Reader::setup(DBManager* db_manager) {
        m_db_manager = db_manager;
        m_dbType = db_manager->getDBType();
    }

    bool Reader::lookup(string key) {
        struct cdb c;
        if (!m_db_manager->readDB(c)) {
            return false;
        }
        
        if (cdb_find(&c, key.c_str(), key.size()) > 0)
           return true;
        else
           return false; 
    }

    // Unused for now
    bool Reader::lookupValue(string key, string& value) {
        struct cdb c;
        if (!m_db_manager->readDB(c)) {
            value = ""; 
            return false;
        }

		char *val;
		unsigned vlen, vpos;

        if (cdb_find(&c, key.c_str(), key.size()) > 0) { // if search successeful
            vpos = cdb_datapos(&c); // position of data in a file
            vlen = cdb_datalen(&c); // length of data
            val = (char*)malloc(vlen * sizeof(char*)); // allocate memory
			cdb_read(&c, val, vlen, vpos); // read the value into buffer
            // handle the value
            val[vlen] = '\0';
			string str_val(val); // This will be the string format of value
			free(val);
            
            value = str_val;
			return true;
		} else {
            value = "";
            return false;
		}
    }

    bool Reader::lookupUnifiedResponse(string key, 
                                       vector<string>& unified_resp) {
        struct cdb c;
        if (!m_db_manager->readDB(c)) { 
            return false;
        }

		char *val;
		unsigned vlen, vpos;
		struct cdb_find cdbf; // structure to hold current find position
		cdb_findinit(&cdbf, &c, key.c_str(), key.size()); // initialize search of key
	
        bool found = false;
        while(cdb_findnext(&cdbf) > 0) {
            vpos = cdb_datapos(&c);
			vlen = cdb_datalen(&c);
				val = (char*)malloc(vlen * sizeof(char*)); // allocate memory
			cdb_read(&c, val, vlen, vpos);
			// handle the value
			val[vlen] = '\0';
				string str_val(val); // This will be the string format of value
			unified_resp.push_back(str_val);
			free(val);
            found = true;
		}

        return found;
    }
}
