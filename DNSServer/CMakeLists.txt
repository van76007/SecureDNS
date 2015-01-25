project(Nameserver)
cmake_minimum_required(VERSION 2.6)

## Build the lib is needed:
#add_definitions(-Wall -ggdb)
add_definitions(-Wall -ggdb)
#Include the include dir for headers. Local to LiveForensics
#include_directories("include")

## Some Libraries are using Boost:
## Make sure boost headers and includes are available
### For now we only use serialization
find_package( Boost REQUIRED serialization filesystem)

## Include local dir "include" and the boost header files
include_directories ("/usr/local/include/" "include "${Boost_INCLUDE_DIRS} )

## Add boost libraries so linker can find them.
link_directories ( "/usr/local/lib/mysql" ${Boost_LIBRARY_DIRS} )

add_library( DNSLib STATIC
	DNSNameserver.cpp
	DNSstructure.cpp
	DNSList.cpp
  )

add_executable(MainDNSserver
	MainDNSserver.cpp
)
target_link_libraries(DNSLib boost_thread boost_filesystem pthread)
target_link_libraries( MainDNSserver
  yaml
  DNSLib  	
  pthread
  mysqlclient
  cdb
)

add_executable(Re
	readP.cpp
)

target_link_libraries(Re
	yaml
	boost_filesystem
	mysqlclient
)

add_executable(Monitor
	monitor.cpp
)

target_link_libraries(Monitor
	yaml
	boost_filesystem
	mysqlclient
	cdb
)
