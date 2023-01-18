#include "entity.h"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "tiny_gltf.h"
#include <string>
#include <vector>
namespace rendersystem
{

struct StandardVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 color;
};

class VisualComponent : public Component
{
  public:
    DEFINE_COMPONENT_ID(VisualComponent);
    VisualComponent()
    {
    }
    const std::vector<StandardVertex>& vertices() const
    {
        return m_vertices;
    };
    const std::vector<uint32_t>& indices() const
    {
        return m_indices;
    };
    std::vector<StandardVertex>& vertices()
    {
        return m_vertices;
    };
    std::vector<uint32_t>& indices()
    {
        return m_indices;
    };

    static std::shared_ptr<VisualComponent> test_triangle();
    static std::shared_ptr<VisualComponent> from_gltf_file(const std::string& fn);

  private:
    std::vector<StandardVertex> m_vertices;
    std::vector<uint32_t> m_indices;
};

std::vector<StandardVertex> create_triangle_data();

template <typename T> std::vector<uint32_t> load_indices_raw(const unsigned char* p_start, const unsigned char* p_end)
{
    std::vector<uint32_t> indices;
    const unsigned char* p_cur = nullptr;
    for (p_cur = p_start; p_cur < p_end; p_cur += sizeof(T))
    {
        uint32_t idx = *(T*)p_cur;
        indices.push_back(idx);
    }
    return indices;
}

template <typename T>
void iterate_accessor(const tinygltf::Accessor& acc, const tinygltf::Model& model, std::function<void(const T& pos)> cb)
{
    const tinygltf::BufferView& view = model.bufferViews[acc.bufferView];
    const tinygltf::Buffer& buf = model.buffers[view.buffer];
    // we are not supporting array-of-structs! The data needs be linear as in ppppp'nnnnnnn'ccccccc, not pnc'pnc'pnc
    if (view.byteStride != 0)
    {
        throw std::runtime_error("gltf file has unsupported interleaving of data.");
    }
    const unsigned char* p_start = buf.data.data() + view.byteOffset + acc.byteOffset;
    const unsigned char* p_cur = nullptr;
    for (p_cur = p_start; p_cur < p_start + view.byteLength; p_cur += sizeof(T))
    {
        cb(*reinterpret_cast<const T*>(p_cur));
    }
}

} // namespace rendersystem