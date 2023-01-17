#include "mesh.h"
#include "tiny_gltf.h"
#include <catch2/catch_test_macros.hpp>
#include <cstring>

using namespace rendersystem;

// TEST_CASE("Mesh gltf")
// {
//     tinygltf::TinyGLTF loader;
//     std::string err;
//     std::string warn;
//     tinygltf::Model model;
//     REQUIRE(loader.LoadASCIIFromFile(&model, &err, &warn, "/home/tlangmo/dev/vulkan-human/assets/torus.gltf"));
//     Mesh m(model);
// }