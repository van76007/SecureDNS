/**
 * @file
 * @author SNU <snu@csis.mu>
 * @Based on Lasse Bach logging function
 * @brief Implementation of logging for Nameserver
 *
 * @section Description
 * Initializes the logging framework log4cxx.
 * Implemented as a simple function. 
 *
 */

#include <ServerException.h>

using namespace log4cxx;

log4cxx::FileAppenderPtr Server_logging(std::string logDomain, std::string logFileName, enum LogLayout loglayout) {
  try {
    log4cxx::FileAppenderPtr fileAppender;
    //Create Layouts for log lines
    //Refer to http://logging.apache.org/log4cxx/apidocs/classlog4cxx_1_1_pattern_layout.html

    // The Debug Layout
    const log4cxx::LogString sDebugPattern (LOG4CXX_STR("%-10r %-5p (%c) - %m %n"));
    log4cxx::LayoutPtr debug_layout(new log4cxx::PatternLayout(sDebugPattern));

    // The Production Layout
    const log4cxx::LogString sProductionPattern (LOG4CXX_STR("%d{dd MMM yyyy HH:mm:ss} - %m %n"));
    log4cxx::LayoutPtr production_layout(new log4cxx::PatternLayout(sProductionPattern));


		
    log4cxx::LayoutPtr layout;
		
    switch(loglayout){
    case DEBUGLOG:
      layout = debug_layout;
      break;
    case PRODUCTIONLOG:
      layout = production_layout;
      break;				
    }
    // Get logger we are configuring
    LoggerPtr logger = Logger::getLogger(logDomain);

    // Remove any existing appenders
    logger->removeAllAppenders();

    //Add the appender
    if(0 == logFileName.compare("console")){
      // We are adding the console appender
      log4cxx::ConsoleAppenderPtr ConsoleAppender(new log4cxx::ConsoleAppender(layout));
      logger->addAppender(ConsoleAppender);
    } else {
      // Adding file appender
      log4cxx::LogString sLogFileName;
      log4cxx::helpers::Transcoder::decode(logFileName, sLogFileName);
      fileAppender = new log4cxx::FileAppender(layout, sLogFileName, true);
      logger->addAppender(fileAppender);
    }
    std::string prod;
    // Set top level loglevel:
#ifndef PRODUCTION
    prod = "Debug";
    logger->setLevel(log4cxx::Level::getTrace());
#else
    prod = "Production";
    logger->setLevel(log4cxx::Level::getInfo());
#endif

    // Get a logger for this function
    logger = Logger::getLogger(logDomain + ".init_logging");

    return fileAppender;
  } catch (std::exception& ) {
    throw;
  }
	
};



