cmake_minimum_required(VERSION 3.16)
project(BANNG)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-mavx -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-Os")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

add_subdirectory(lib/pybind11)

include_directories(include/)
add_subdirectory(src/)
