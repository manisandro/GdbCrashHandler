cmake_minimum_required(VERSION 2.8.11)

project(GdbCrashHandler)

set(LIB_INSTALL_DIR lib${LIB_SUFFIX} CACHE PATH "Library installation dir")

set(GDBCH_LIB_MAJOR 0)
set(GDBCH_LIB_MINOR 0)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(QuaZip5 REQUIRED)
set(CMAKE_AUTOMOC ON)

add_subdirectory(lib)
add_subdirectory(example)
