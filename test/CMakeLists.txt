cmake_minimum_required(VERSION 3.23)

project(di_ptr_test CXX)

enable_testing()

add_executable(${PROJECT_NAME} di_ptr_test.cpp)
find_package(Catch2 2 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2 di_ptr)
include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME})