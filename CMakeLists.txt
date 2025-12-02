cmake_minimum_required(VERSION 4.1)
project(f CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE src
    src/*.ixx)

add_library(f STATIC ${src})
