include_directories(${CURRENT_SOURCE_DIR})

find_package(SDL2 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

message(${Vulkan_INCLUDE_DIR})


include_directories(${SDL2_INCLUDE_DIRS} ../../thirdparty/vk-bootstrap/src)


add_library(engine OBJECT
                engine.cpp
                swapchain.cpp
                pass.cpp
                core.cpp
                mesh.cpp
                pipeline.cpp
)

