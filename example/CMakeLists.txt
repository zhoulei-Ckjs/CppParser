cmake_minimum_required(VERSION 3.10)
project(example)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_CURRENT_LIST_DIR})

add_executable(${PROJECT_NAME} example.cpp)