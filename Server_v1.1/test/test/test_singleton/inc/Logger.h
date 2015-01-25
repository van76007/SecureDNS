// Ref: http://www.yolinux.com/TUTORIALS/C++Singleton.html
// Delete pointer: http://www.infernodevelopment.com/singleton-c

#ifndef __LOGGER_H__
#define __LOGGER__

#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;

namespace screenDNS {

class Logger{
public:
   static Logger* Instance();
   
   bool openLogFile(std::string logFile);
   void writeToLogFile(std::string msg);
   bool closeLogFile(); 

private:
   Logger(){}; // 
   Logger(Logger const&){};             // copy constructor is private
   Logger& operator=(Logger const&){};  // assignment operator is private
   static Logger* m_pInstance;
   
   ofstream logfile;
};

}
#endif /* __LOGGER_H__ */