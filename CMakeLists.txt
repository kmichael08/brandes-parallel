cmake_minimum_required(VERSION 3.5)
project(brandes_parallel)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O2 -Wall")

set(SOURCE_FILES
    brandes.cpp
    )

add_executable(brandes_parallel ${SOURCE_FILES})