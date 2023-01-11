cmake_minimum_required(VERSION 3.0.0)
project(vulkan-human VERSION 0.1.0)


add_executable(vulkan-human src/main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_subdirectory(tests)