project(Nameserver)
cmake_minimum_required(VERSION 2.6)

## Build the lib is needed:
add_definitions(-Wall -ggdb)

#Include the include dir for headers. Local to LiveForensics
#include_directories("include")

#add_library( DNSLib STATIC
#	DNSNameserver.cpp
#  )

## Some Libraries are using Boost:
## Make sure boost headers and includes are available
### For now we only use serialization
find_package( Boost REQUIRED serialization filesystem)

## Include local dir "include" and the boost header files
include_directories ("include" ${Boost_INCLUDE_DIRS} )

## Add boost libraries so linker can find them.
link_directories ( ${Boost_LIBRARY_DIRS} )

## Add boost libraries so linker can find them.
link_directories ( "/usr/local/lib/mysql" ${Boost_LIBRARY_DIRS} )

add_executable( Serv
	server.cpp
)

add_executable( Mon
	monitor.cpp
)

target_link_libraries( Mon
	boost_filesystem
	cdb
	mysqlclient
	boost_thread
	yaml
)

add_executable(CheckMonitor
	checkmon.cpp
)

target_link_libraries(CheckMonitor
	cdb
	mysqlclient
)

add_executable(	Lock
	boostlock.cpp
)

target_link_libraries( Lock
	boost_thread
)

target_link_libraries( Serv
	mysqlclient
)

add_executable(Re
	readP.cpp
)

add_executable(Wr
	writeP.cpp
)

target_link_libraries(Re
	boost_filesystem
)

#add_library( Lib STATIC
#	ServerLogging.cpp
#)

#add_executable( Tlogxx
#	testlogxx.cpp
#)

#target_link_libraries(Tlogxx
#	Lib
#	log4cxx
#)

add_executable(CT
	cdbtest.cpp
)
target_link_libraries(CT
	cdb
	boost_filesystem
)

add_executable(CL	
	client.cpp
)

add_executable(Booth
	boosthread.cpp
)
target_link_libraries(Booth
	boost_thread
	pthread
	
)

add_executable(TB
	testboo.cpp
)
target_link_libraries(TB
	boost_thread
	pthread
)
