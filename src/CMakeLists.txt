include_directories(${CURRENT_SOURCE_DIR} rendersystem components inputsystem)

find_package(SDL2 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_subdirectory(components)   
add_subdirectory(rendersystem)
add_subdirectory(inputsystem)



add_executable(main main.cpp)


target_link_libraries(main
    PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    vk-bootstrap::vk-bootstrap
    ${Vulkan_LIBRARY}
    glfw
    $<TARGET_OBJECTS:rendersystem>
    $<TARGET_OBJECTS:components>
    $<TARGET_OBJECTS:inputsystem>
)
