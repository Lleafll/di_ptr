cmake_minimum_required(VERSION 3.23)

project(examples CXX)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE maybe_owning_ptr)