include_directories(${CURRENT_SOURCE_DIR} ${CURRENT_SOURCE_DIR}/pipelines )

find_package(SDL2 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
message(${Vulkan_FOUND})
    

add_executable(main main.cpp
                    engine.cpp
                    swapchain.cpp
                    mesh.cpp
                    pipelines/basic_pipeline.cpp
)


target_link_libraries(main
    PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    vk-bootstrap::vk-bootstrap
    ${Vulkan_LIBRARY}
    glfw
)
add_subdirectory(shaders)


