get_filename_component(PARENT_DIR ../ ABSOLUTE)
#include_directories(${CURRENT_SOURCE_DIR} ${PARENT_DIR})

find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

#add_subdirectory(tests)
include_directories(${GLFW3_INCLUDE_DIRS} )

add_library(inputsystem OBJECT
                inputsystem.cpp
)

target_link_libraries(inputsystem
    PRIVATE
    glfw
)