cmake_minimum_required(VERSION 3.23)

project(di_ptr CXX)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} INTERFACE examples/main.cpp)
target_include_directories(${PROJECT_NAME} INTERFACE include)

add_subdirectory(examples)
add_subdirectory(test)
