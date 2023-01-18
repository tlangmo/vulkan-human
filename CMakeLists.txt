cmake_minimum_required(VERSION 3.7)
project(vulkan-human VERSION 0.1.0)

enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_STANDARD 17)
include(CPack)



set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
file(COPY "assets" DESTINATION "${PROJECT_BINARY_DIR}")



add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(thirdparty/vk-bootstrap)