/*
Author: dvv@avauntguard.com
Idea: Testbed for boost snippets
*/
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

#include <vector>
#include <ctime>
#include <string.h>

#define MAIN
#include "Logger.h"
#include "Worker.h"

using namespace std;
using namespace screenDNS;

// Idead: Passing logger as pointer and call log method between different threads
// No need anymore as we can call Logger anywhere from any thread. Just including Logger.h in them
int main(int argc, char* argv[])
{
    std::cout << "Begin test singleton\n";    
    
	// Usage: Logger::Instance()->openLogFile("logFile.txt");
	//        Logger::Instance()->writeToLogFile("a Foo");
	//        Logger::Instance()->closeLogFile();
	
	Logger::Instance()->openLogFile("logFile.txt");
	
	// Start multiple worker threads
	boost::thread_group workers;
    for (int i=0; i<50; i++) {
        screenDNS::Worker w(i);
        workers.create_thread(w);
    }
	
	sleep(120); // sleep for 120s
	
	workers.interrupt_all(); workers.join_all();    
    std::cout << "All worker threads stopped" << std::endl;
	
	Logger::Instance()->closeLogFile();
	
    std::cout << "End test singleton\n";      
}
