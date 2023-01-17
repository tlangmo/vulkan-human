#include "visual.h"
#include <iostream>
// #define TINYGLTF_IMPLEMENTATION
// #define STB_IMAGE_IMPLEMENTATION
// #define STB_IMAGE_WRITE_IMPLEMENTATION
// // #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
// #include "tiny_gltf.h"

namespace rendersystem
{
GLTFComponent::GLTFComponent(const std::string& fn)
{
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;
    bool ret = loader.LoadASCIIFromFile(&m_model, &err, &warn, fn);
    if (!warn.empty())
    {
        std::cout << "GLTF warning: " << warn << std::endl;
    }

    if (!err.empty())
    {
        std::cout << "GLTF error: " << err << std::endl;
    }

    if (!ret)
    {
        throw std::runtime_error("cannot load gltf file");
    }
}
} // namespace rendersystem