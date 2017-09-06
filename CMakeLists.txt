cmake_minimum_required(VERSION 2.8.11)

# Project properties
project(GdbCrashHandler)
set(GDBCH_VERSION 0.1.0)


# Soname
# MAJOR is incremented when symbols are removed or changed in an incompatible way
# MINOR is incremented when new symbols are added
set(GDBCH_LIB_MAJOR 1)
set(GDBCH_LIB_MINOR 0)


# Variables
set(LIB_INSTALL_DIR lib${LIB_SUFFIX} CACHE PATH "Library installation dir")
set(INCLUDE_INSTALL_DIR include CACHE PATH "Header installation dir")

string(REGEX REPLACE "^${CMAKE_INSTALL_PREFIX}/" "" PC_INCLUDE_DIR ${INCLUDE_INSTALL_DIR})
string(REGEX REPLACE "^${CMAKE_INSTALL_PREFIX}/" "" PC_LIB_DIR ${LIB_INSTALL_DIR} )


# Dependencies
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5LinguistTools REQUIRED)
find_path(QUAZIP_INCLUDE_DIR NAMES quazipfile.h PATH_SUFFIXES quazip5 quazip)
find_library(QUAZIP_LIBRARIES quazip5)
set(CMAKE_AUTOMOC ON)

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_TOLOWER)
if(NOT CMAKE_BUILD_TYPE_TOLOWER MATCHES "debug")
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()


# Subdirectories
add_subdirectory(lib)
add_subdirectory(example)
