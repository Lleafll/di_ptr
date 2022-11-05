cmake_minimum_required(VERSION 3.23)

project(di_ptr_test CXX)

enable_testing()

add_executable(${PROJECT_NAME}
    di_ptr_test.cpp
    forwarddeclared.cpp)
find_package(Catch2 3 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2WithMain di_ptr)
include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME})