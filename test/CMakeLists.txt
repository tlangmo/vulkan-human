cmake_minimum_required(VERSION 3.2)
project (motesque_unittest)

find_package(Catch2 CONFIG REQUIRED)

add_executable(run_tests test.cpp)
target_link_libraries(run_tests PRIVATE  Catch2::Catch2WithMain)

add_test(run_tests run_tests)