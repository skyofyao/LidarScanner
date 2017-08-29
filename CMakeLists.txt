cmake_minimum_required(VERSION 2.8.11)
project(pheno3 CXX)
set(CMAKE_BUILD_TYPE "RELEASE")
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories("/usr/local/include/urg_cpp" "/usr/include/flycapture" "${CMAKE_SOURCE_DIR}/src")
link_directories("/usr/local/lib/")
file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)
message(STATUS "SRC:" ${SRC_FILES})


add_executable(pheno3v2 ${SRC_FILES})
target_link_libraries(pheno3v2 urg_cpp.so flycapture.so)