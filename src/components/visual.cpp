
#include "visual.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"
#include <iostream>

namespace rendersystem
{

std::shared_ptr<VisualComponent> VisualComponent::make_triangle()
{
    auto comp = std::make_shared<VisualComponent>();
    std::vector<StandardVertex>& vertices = comp->vertices();
    vertices.resize(3);
    vertices[0].position = {1.f, 1.f, 0.0f};
    vertices[1].position = {-1.f, 1.f, 0.0f};
    vertices[2].position = {0.f, -1.f, 0.0f};

    vertices[0].normal = {1.f, 0.f, 0.0f};
    vertices[1].normal = {0.f, 1.f, 0.0f};
    vertices[2].normal = {0.f, 0.f, 1.0f};

    vertices[0].color = {1.f, 0.f, 0.0f};
    vertices[1].color = {0.f, 1.f, 0.0f};
    vertices[2].color = {0.f, 0.f, 1.0f};
    comp->indices() = {0, 1, 2};
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
    // we only support single meshes for now. Validate
    if (model.meshes.size() > 1 || model.meshes[0].primitives.size() > 1)
    {
        throw std::runtime_error("gltf file contains more than one mesh! unsupported.");
    }

    auto comp = std::make_shared<VisualComponent>();
    std::vector<StandardVertex>& vertices = comp->vertices();

    // // extract the indicies for the first model;
    const tinygltf::Mesh& m = model.meshes[0];
    const tinygltf::Primitive& p = m.primitives[0];
    auto& acc_index = model.accessors[p.indices];
    const tinygltf::BufferView& index_view = model.bufferViews[acc_index.bufferView];
    const tinygltf::Buffer& index_buffer = model.buffers[index_view.buffer];

    if (acc_index.type == TINYGLTF_TYPE_SCALAR)
    {
        const unsigned char* p_start = index_buffer.data.data() + index_view.byteOffset + acc_index.byteOffset;
        const unsigned char* p_end = p_start + index_view.byteLength;
        comp->indices() = (acc_index.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                              ? load_indices_raw<uint16_t>(p_start, p_end)
                              : load_indices_raw<uint32_t>(p_start, p_end);
    }

    {
        auto& acc = model.accessors[p.attributes.at("POSITION")];
        vertices.resize(acc.count);
        StandardVertex* p_va = vertices.data();
        iterate_accessor<glm::vec3>(acc, model, [&p_va](const glm::vec3& pos) {
            p_va->position = pos;
            p_va->color = glm::vec3(1.0, 0.0, 1.0);
            p_va++;
        });
    }
    {
        StandardVertex* p_va = vertices.data();
        auto& acc = model.accessors[p.attributes.at("NORMAL")];
        if (acc.count != vertices.size())
        {
            throw std::runtime_error("gltf attributes are of inconsistent size.unsupported!");
        }
        iterate_accessor<glm::vec3>(acc, model, [&vertices, &p_va](const glm::vec3& normal) {
            p_va->normal = normal;
            p_va++;
        });
    }
    {
        StandardVertex* p_va = vertices.data();
        auto& acc = model.accessors[p.attributes.at("TEXCOORD_0")];
        if (acc.count != vertices.size())
        {
            throw std::runtime_error("gltf attributes are of inconsistent size.unsupported!");
        }
        iterate_accessor<glm::vec3>(acc, model, [&vertices, &p_va](const glm::vec2& tex_coords) {
            p_va->uv = tex_coords;
            p_va++;
        });
    }

    std::cout << "Mesh stats:\n#vertices:\t" << comp->vertices().size() << "\n#indices:\t" << comp->indices().size()
              << std::endl;

    return comp;
}

} // namespace rendersystem