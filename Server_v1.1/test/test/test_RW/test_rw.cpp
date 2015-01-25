#define MAIN // Trick to prevent re-definition of classes "DBManager" in "Reader" and "Monitor" classes
#include "DBManager.h"
#include "Monitor.h"
#include "Reader.h"

using namespace std;
using namespace screenDNS;

/*
  // How To Test:
  + Create an empty file pretending DB log file: touch ~/foo
  + Start test_reader_writer: ./test_rw
  + Or firstly, start process that modify DB log file: ./test_overwrite
  + Note: must compile this binary seperately: g++ -o test_overwrite test_overwrite.c 
*/
int main(void) {
    
    // New design
    /* Step 0: 
             + Create DBManager class to provide service read/write DB
             + Start a monitor thread to get nofication on change of LOG file.
             + Start a group of Readers need to invoke readDB() method of DBManager
    */
    DBManager* db_manager = new DBManager(); // Should implement singleton here

    // Start a monitor thread
    screenDNS::Monitor monitor(db_manager);
    boost::thread t_monitor(monitor);    
    
    // Start a group of 50 Reader threads
    boost::thread_group readers;
    for (int i=0; i<50; i++) {
        screenDNS::Reader r(i, db_manager);
        readers.create_thread(r);
    }

    /* Step 1:
             + Wait for monitor thread stop running after 3 mins
             + Stop the group of Readers after that
             + monitor thread and group of Readers share the same instance of DBManager
     */
    // Let the monitor thread run for 3mins, then try to interrupt it and terminate the program
    while (t_monitor.timed_join(boost::posix_time::seconds(180)) == false)
    {
        std::cout << "Thread not stopped. Interrupt it now" << std::endl;
        t_monitor.interrupt();
        std::cout << "Thread interrupt request sent" << std::endl;
        std::cout << "Wait for 10 mins again" << std::endl;
    }
      
    //
    std::cout << "Thread t_monitor stopped" << std::endl;

    // Stop the readers group
    readers.interrupt_all(); readers.join_all();    
    std::cout << "All Reader threads stopped" << std::endl;
    
    // Rest in peace
    return 0;
}
