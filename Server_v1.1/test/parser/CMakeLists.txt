## mkdir build
## cd build
## cmake ..
## make VERBOSE=1 -k 2>&1 | tee make.log
## gedit make.log for more infor
## rm -rf * if need to re-make

project(SCREENDNS)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_FLAGS "-g -Wall")

find_package( Boost REQUIRED serialization filesystem)

message(STATUS "Source_dir is: " ${SCREENDNS_SOURCE_DIR})
message(STATUS "Boost_INCLUDE_DIRS is: " ${Boost_INCLUDE_DIRS})
message(STATUS "Boost_LIBRARY_DIRS is: " ${Boost_LIBRARY_DIRS})

include_directories(${Boost_INCLUDE_DIRS} "${SCREENDNS_SOURCE_DIR}/inc")
link_directories ( ${Boost_LIBRARY_DIRS} )

add_library(libdns_util STATIC dns_util.cpp)
SET_PROPERTY(
   TARGET libdns_util
   PROPERTY COMPILE_DEFINITIONS INFO DBUG
   )

add_executable(test_dns_util test_dns_util.cpp)
target_link_libraries(test_dns_util libdns_util boost_system)  
set(CMAKE_BUILD_TYPE Debug)

