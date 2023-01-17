include_directories(${CURRENT_SOURCE_DIR})

set(SOURCES 
    ../mesh.cpp   
    mesh.t.cpp
)

find_package(Catch2 CONFIG REQUIRED)
target_include_directories(components PRIVATE ${TINYGLTF_INCLUDE_DIRS})

add_executable(rendersystem_test ${SOURCES})
target_link_libraries(rendersystem_test PRIVATE 
        Catch2::Catch2WithMain
        ${Vulkan_LIBRARY}
)

target_compile_definitions(rendersystem_test PRIVATE -DVMA_IMPLEMENTATION)  # -D removed


add_test(rendersystem_test rendersystem_test)

