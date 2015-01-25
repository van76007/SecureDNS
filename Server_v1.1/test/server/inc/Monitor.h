#ifndef __MONITOR_H__
#define __MONITOR__

/*
* Only available on FreeBSD: g++ -o test_kqueue test_kqueue.c
*/

/*
The program will monitor changes to the /usr/home/dvv/foo file 
and will print messages whenever it is deleted, modified or their attributes change. 
The program finishes when the file being monitoring is deleted.

The steps to use kqueue are the following:

   1. Call kqueue(2) to create a new kernel event queue. The descriptor it returns will be later used by kevent(2).
   
   2. Open the file to monitor and keep its descriptor around. We'll need this to attach an event monitor to it.
   
   3. Initialize a vector of struct kevent elements that describes the changes to monitor. 
   Since we are only monitoring a single file, we need a one-element vector. 
   This vector is filled up with calls to the EV_SET macro. 
   This macro takes: the descriptor of the kqueue, 
   the descriptor of the file to monitor (ident), 
   the filter to apply to it, 
   several flags and optional arguments to the filter. 
   Note that an entry in this table is identified by its ident/filter pair.
   
   4. Call the kevent(2) function. 
   This system call takes the list of changes to monitor we constructed before 
   and does not return until at least one event is received (or when an associated timeout is exhausted). 
   The function returns the number of changes received and stores information about them 
   in another vector of struct kevent elements (we'll only get notifications of one event at a time, 
   hence we don't use a vector, but a simple variable).
   
   5. Interpret the results. 
   If kevent(2) returned a number greater than 0, 
   we have to inspect the output vector and see which events were received. 
   Each filter has its semantics about the results. 
   For example, we are using the EVFILT_VNODE filter, 
   which takes a list of conditions to monitor in the fflags field and 
   modifies it to include only the conditions that triggered the filter.
*/
// For kqueue() notification
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h> 
#include <sys/fcntl.h>

// For other stuffs
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>

#ifndef MAIN
#include "DBManager.h"
#endif

using namespace std;
using namespace screenDNS;

namespace screenDNS {

    class Monitor {
    public:
        // Constructor
        Monitor(DBManager* db_manager);
        
        // Callable from another thread
        void operator () (); 

    private:
        DBManager* m_db_manager;
        DB_t m_dbType;
        string m_cdbDirPath; 
    };
}

#endif /* __MONITOR_H__ */
