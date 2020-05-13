cmake_minimum_required (VERSION 3.1)
project(configure_and_connect)

if (WIN32)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /EHsc")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd /EHsc")
endif (WIN32)

if (UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3 -O3")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -Wall")
endif (UNIX)

set(PINCHOT_API_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../..")
include(CAPISources)
include(VersionInfo)

add_executable(configure_and_connect
  ${CMAKE_CURRENT_SOURCE_DIR}/src/configure_and_connect.cpp
  ${C_API_SOURCES})
