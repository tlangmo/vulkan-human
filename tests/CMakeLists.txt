cmake_minimum_required(VERSION 3.2)
project (motesque_unittest)

include_directories(${CURRENT_SOURCE_DIR} )

add_executable(run_tests test.cpp
)

