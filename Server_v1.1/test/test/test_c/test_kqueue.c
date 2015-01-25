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
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h> 
#include <sys/fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Note: The program will exit if the file does not exist! So the file "/usr/home/dvv/foo"(empty) must exist before
int main(void)
{
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
   
   int c = 0;
   for (;;) {
       nev = kevent(kq, &change, 1, &event, 1, NULL);
       if (nev == -1)
           perror("kevent");
       else if (nev > 0) {
           if (event.fflags & NOTE_DELETE) {
               printf("File deleted\n");
               break;
           }
		   
           if (event.fflags & NOTE_EXTEND ||
               event.fflags & NOTE_WRITE)
		   { 	   
               printf("File modified\n", c);
			   // Read file content but DO NOT REUSE f
			   std::string line;
			   ifstream myfile ("/usr/home/dvv/foo");
			   getline (myfile,line);
               std::cout << "Get file content: " << line << std::endl;
		   }
       }
   }

   close(kq);
   close(f);
   return EXIT_SUCCESS;
}

