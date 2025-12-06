cmake_minimum_required(VERSION 4.1)
project(f-lib CXX)
include(cmake/f.cmake)

add_subdirectory(modules/f)

if (${CMAKE_CXX_PLATFORM_ID} STREQUAL Windows)
    add_subdirectory(modules/f.win32)
endif ()