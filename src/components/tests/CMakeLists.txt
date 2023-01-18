include_directories(${CURRENT_SOURCE_DIR})

set(SOURCES 
    ../visual.cpp   
    coordsys.t.cpp
    visual.t.cpp
    entity.t.cpp
)
find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")
find_package(Catch2 CONFIG REQUIRED)


add_executable(components_test ${SOURCES})
target_link_libraries(components_test PRIVATE 
        Catch2::Catch2WithMain
)
target_include_directories(components_test PRIVATE ${TINYGLTF_INCLUDE_DIRS})
add_test(components_test components_test)

