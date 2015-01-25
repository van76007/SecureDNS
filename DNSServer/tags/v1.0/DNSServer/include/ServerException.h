/**
 * @file
 * @author snu <snu@csis.mu>
 *
 * @brief Header file for logging initialization
 *
 * @section Description
 * Initializes the logging framework log4cxx.
 * Implemented as a simple function. 
 *
 */
#ifndef NameServer_INITLOGGING_H
#define NameServer_INITLOGGING_H

#include <log4cxx/logstring.h>
#include <stdlib.h>
#include <log4cxx/logger.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/helpers/transcoder.h>
#include <log4cxx/level.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/simplelayout.h>
#include <log4cxx/patternlayout.h>
#include <iostream>


/**
 * @brief Defines the available LogLayouts
 */
enum LogLayout { DEBUGLOG, PRODUCTIONLOG };

/**
 * @brief Initialize a logger
 *
 * @param logDomain The logger which recieves the information
 * @param logFileName The log file or the string "console" if console logging is desired
 * @param LogLayout The logLayout, See LogLayout.
 *
 */
log4cxx::FileAppenderPtr Server_logging(std::string logDomain, std::string logFileName, enum LogLayout loglayout);

#endif //NameServer_INITLOGGING_H
