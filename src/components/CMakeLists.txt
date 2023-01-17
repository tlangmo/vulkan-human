include_directories(${CURRENT_SOURCE_DIR})


find_package(Catch2 CONFIG REQUIRED)

find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")
add_library(components OBJECT
                visual.cpp
)
target_include_directories(components PRIVATE ${TINYGLTF_INCLUDE_DIRS})

add_executable(components_test tests/entity.t.cpp)
target_link_libraries(components_test PRIVATE  Catch2::Catch2WithMain)
add_test(components_test components_test)

