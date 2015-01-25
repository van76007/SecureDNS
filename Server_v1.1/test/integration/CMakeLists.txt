## mkdir build
## cd build
## cmake ..
## make VERBOSE=1 -k 2>&1 | tee make.log
## gedit make.log for more infor
## rm -rf * if need to re-make

project(SCREENDNS)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_FLAGS "-g -Wall")

find_package( Boost REQUIRED serialization filesystem system)

set(Boost_INCLUDE_DIRS "/usr/local/include")
set(Boost_LIBRARY_DIRS "/usr/local/lib")

message(STATUS "Source_dir is: " ${SCREENDNS_SOURCE_DIR})
message(STATUS "Boost_INCLUDE_DIRS is: " ${Boost_INCLUDE_DIRS})
message(STATUS "Boost_LIBRARY_DIRS is: " ${Boost_LIBRARY_DIRS})

include_directories(${Boost_INCLUDE_DIRS} "${SCREENDNS_SOURCE_DIR}/include")
link_directories ( ${Boost_LIBRARY_DIRS} )

add_library(libserver STATIC server.cpp forwarder.cpp dns_util.cpp)

SET_PROPERTY(
   TARGET libserver
   PROPERTY COMPILE_DEFINITIONS INFO DBUG #THREAD
   )

add_executable(main_server main_server.cpp)
target_link_libraries(main_server libserver boost_system boost_thread boost_date_time)  
set(CMAKE_BUILD_TYPE Debug)
