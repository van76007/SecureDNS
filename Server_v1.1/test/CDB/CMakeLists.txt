project(SCREENDNS)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_FLAGS "-g -Wall")

find_package( Boost REQUIRED serialization filesystem system)

set(Boost_INCLUDE_DIRS "/usr/local/include")
set(Boost_LIBRARY_DIRS "/usr/local/lib")

message(STATUS "Source_dir is: " ${SCREENDNS_SOURCE_DIR})
message(STATUS "Boost_INCLUDE_DIRS is: " ${Boost_INCLUDE_DIRS})
message(STATUS "Boost_LIBRARY_DIRS is: " ${Boost_LIBRARY_DIRS})

include_directories(${Boost_INCLUDE_DIRS} "${SCREENDNS_SOURCE_DIR}/inc")
link_directories ( "/usr/local/lib/mysql" ${Boost_LIBRARY_DIRS} )

add_library(libCDB_Generator STATIC Config.cpp)

add_executable (
	cdb_generator
	main.cpp
)

target_link_libraries (
	cdb_generator
	libCDB_Generator
  	boost_filesystem
    boost_program_options	
  	pthread
  	mysqlclient
  	cdb
)

## For debugging by GDB  
set(CMAKE_BUILD_TYPE Debug)