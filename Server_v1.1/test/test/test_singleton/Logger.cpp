#include "Logger.h"

namespace screenDNS {
	// Global static pointer used to ensure a single instance of the class.
	Logger* Logger::m_pInstance = NULL;  

	/** This function is called to create an instance of the class. 
		Calling the constructor publicly is not allowed. The constructor 
		is private and is only called by this Instance function.
	*/
	  
	Logger* Logger::Instance()
	{
	   if (!m_pInstance)   // Only allow one instance of class to be generated.
		  m_pInstance = new Logger;

	   return m_pInstance;
	}

	bool Logger::openLogFile(std::string _logFile)
	{
	    logfile.open(_logFile.c_str());
	}
	
	void Logger::writeToLogFile(std::string msg)
	{
	    logfile << msg << "\n";
	}
	
    bool Logger::closeLogFile()
	{
	    logfile.close();
	}
}
