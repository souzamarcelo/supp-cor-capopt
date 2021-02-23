cmake_minimum_required(VERSION 2.8)
include (InstallRequiredSystemLibraries)

project(hhbqp)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unknown-pragmas -fPIC -DIL_STD -fexceptions -std=c++0x -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native -mtune=native -openmp -O3 -DNDEBUG")

find_package(Boost 1.50.0 COMPONENTS program_options timer system chrono filesystem)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

add_executable(hhbqp hhbqp.cpp)
target_link_libraries(hhbqp ${Boost_LIBRARIES} rt)
