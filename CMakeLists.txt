cmake_minimum_required(VERSION 3.23)

project(maybe_owning_ptr CXX)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include)

add_subdirectory(test)
