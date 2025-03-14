cmake_minimum_required (VERSION 3.1)
project (pinchot)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)

add_compile_definitions(WIN_EXPORT)
if (MSVC)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /EHsc")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd /EHsc")
endif (MSVC)

if (UNIX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3 -O3")
  # The "-fvisibility=hidden" flag makes all functions in the shared library
  # not exported by default. To export a function, it needs to be done so
  # explicitly using toolchain specific function properties.
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
endif (UNIX)

set(PINCHOT_API_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
include(CAPISources)
include(VersionInfo)

add_library(pinchot ${C_API_SOURCES})
target_include_directories(pinchot PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)
target_link_libraries(pinchot ${CMAKE_THREAD_LIBS_INIT})
if (WIN32)
    #find_library(WSOCK32_LIBRARY wsock32)
    #find_library(WS2_32_LIBRARY ws2_32)
    target_link_libraries(pinchot wsock32 ws2_32 iphlpapi)
endif (WIN32)

install(TARGETS pinchot
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
