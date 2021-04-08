cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")

project(EVO)

add_executable(exec main.cpp chromosome.cpp gate.cpp table.cpp)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")