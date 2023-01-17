get_filename_component(PARENT_DIR ../ ABSOLUTE)
#include_directories(${CURRENT_SOURCE_DIR} ${PARENT_DIR})

find_package(SDL2 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)




message(${Vulkan_INCLUDE_DIR})

find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")
include_directories(${SDL2_INCLUDE_DIRS} ../../thirdparty/vk-bootstrap/src ${PARENT_DIR} )

add_subdirectory(tests)
add_subdirectory(shaders)

add_library(rendersystem OBJECT
                rendersystem.cpp
                swapchain.cpp
                pass.cpp
                core.cpp
                mesh.cpp
                pipeline.cpp
)

target_include_directories(rendersystem PRIVATE ${TINYGLTF_INCLUDE_DIRS})
add_dependencies(rendersystem shaders)
