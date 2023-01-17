#include "visual.h"
#include <iostream>

namespace rendersystem
{

std::shared_ptr<VisualComponent> VisualComponent::test_triangle()
{
    auto comp = std::make_shared<VisualComponent>();
    std::vector<StandardVertex>& vertices = comp->vertices();
    vertices.resize(3);
    vertices[0].position = {1.f, 1.f, 0.0f};
    vertices[1].position = {-1.f, 1.f, 0.0f};
    vertices[2].position = {0.f, -1.f, 0.0f};

    vertices[0].color = {1.f, 0.f, 0.0f};
    vertices[1].color = {0.f, 1.f, 0.0f};
    vertices[2].color = {0.f, 0.f, 1.0f};
    return comp;
}

std::shared_ptr<VisualComponent> VisualComponent::from_gltf_file(const std::string& fn)
{
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;
    tinygltf::Model model;
    if (loader.LoadASCIIFromFile(&model, &err, &warn, fn) == false)
    {
        throw std::runtime_error("cannot load gltf file");
    }
    auto comp = std::make_shared<VisualComponent>();
    std::vector<StandardVertex>& vertices = comp->vertices();
    // // extract the indicies for the first model;
    for (const tinygltf::Mesh& m : model.meshes)
    {
        for (const tinygltf::Primitive& p : m.primitives)
        {
            StandardVertex va;
            {
                auto& acc = model.accessors[p.attributes.at("POSITION")];
                const tinygltf::BufferView& view = model.bufferViews[acc.bufferView];
                const tinygltf::Buffer& buf = model.buffers[view.buffer];
                const unsigned char* p_start = buf.data.data() + view.byteOffset + acc.byteOffset;
                const unsigned char* p_cur = nullptr;
                for (p_cur = p_start; p_cur < p_start + view.byteLength; p_cur += sizeof(float) * 3)
                {
                    glm::vec3 pos = *reinterpret_cast<const glm::vec3*>(p_cur);
                    va.position = *reinterpret_cast<const glm::vec3*>(p_cur);
                    vertices.push_back(va);
                    // std::cout << "[" << va.position.x << "," << va.position.y<< "," << va.position.z << "]" <<
                    // std::endl;
                }
            }
            StandardVertex* p_va = vertices.data();
            {
                auto& acc = model.accessors[p.attributes.at("NORMAL")];
                const tinygltf::BufferView& view = model.bufferViews[acc.bufferView];
                const tinygltf::Buffer& buf = model.buffers[view.buffer];
                const unsigned char* p_start = buf.data.data() + view.byteOffset + acc.byteOffset;
                const unsigned char* p_cur = nullptr;
                for (p_cur = p_start; p_cur < p_start + view.byteLength; p_cur += sizeof(float) * 3)
                {
                    glm::vec3 normal = *reinterpret_cast<const glm::vec3*>(p_cur);
                    p_va->normal = *reinterpret_cast<const glm::vec3*>(p_cur);
                    p_va->color = glm::vec3(0.0, 0.0, 1.0);
                    p_va++;
                    // std::cout << "[" << va.normal.x << "," << va.normal.y<< "," << va.normal.z << "]" << std::endl;
                }
            }
        }
        std::cout << "Mesh vertex count: " << vertices.size() << std::endl;
    }

    return comp;
}

} // namespace rendersystem