#include "tiny_gltf.h"
#include "visual.h"
#include <catch2/catch_test_macros.hpp>
#include <cstring>

using namespace rendersystem;

TEST_CASE("from gltf")
{
    VisualComponent::from_gltf_file("/home/tlangmo/dev/vulkan-human/assets/torus.gltf");
}