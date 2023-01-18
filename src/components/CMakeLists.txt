include_directories(${CURRENT_SOURCE_DIR})


find_package(Catch2 CONFIG REQUIRED)

find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")
add_library(components OBJECT
                visual.cpp
)
target_include_directories(components PRIVATE ${TINYGLTF_INCLUDE_DIRS})
add_subdirectory(tests)

