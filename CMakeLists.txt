cmake_minimum_required (VERSION 3.1)
project (pinchot)

if (WIN32)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /EHsc")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd /EHsc")
endif (WIN32)

if (UNIX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3 -O3")
  # The "-fvisibility=hidden" flag makes all functions in the shared library
  # not exported by default. To export a function, it needs to be done so
  # explicitly using toolchain specific function properties.
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fvisibility=hidden")
endif (UNIX)

set(PINCHOT_API_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
include(CAPISources)
include(VersionInfo)

add_library(pinchot SHARED ${C_API_SOURCES})
target_link_libraries(pinchot ${CMAKE_THREAD_LIBS_INIT})
install(TARGETS pinchot DESTINATION ${SRC_DIR})
install(TARGETS pinchot DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES joescan_pinchot.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})