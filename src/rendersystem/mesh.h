#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>

// forward decl
VK_DEFINE_HANDLE(VmaAllocation)
VK_DEFINE_HANDLE(VmaAllocator)

namespace tinygltf
{
class Model;
}

namespace rendersystem
{

struct VertexAttributes
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 color;
};

struct VertexInputDescriptionData
{
    std::vector<VkVertexInputBindingDescription> bindings;
    std::vector<VkVertexInputAttributeDescription> attributes;
    VkPipelineVertexInputStateCreateFlags flags = 0;
};

/**
 * Mesh structure suitable to draw via the command buffer
 */
class Mesh
{
  public:
    Mesh() : m_vertex_attributes(), m_vertex_buffer(), m_index_buffer(), m_vb_allocation(), m_ib_allocation()
    {
    }
    Mesh(const Mesh& rhs) = delete;
    Mesh(const Mesh&& rhs) = delete;
    std::vector<VertexAttributes>& vertices()
    {
        return m_vertex_attributes;
    }
    const std::vector<VertexAttributes>& vertices() const
    {
        return m_vertex_attributes;
    }
    std::vector<uint32_t>& indices()
    {
        return m_indices;
    }
    const std::vector<uint32_t>& indices() const
    {
        return m_indices;
    }
    VkBuffer& vb()
    {
        return m_vertex_buffer;
    }
    VkBuffer& ib()
    {
        return m_index_buffer;
    }
    void create(VmaAllocator vma_allocator);
    void destroy(VmaAllocator vma_allocator);

  public:
    static VertexInputDescriptionData& get_vertex_input_description();

  private:
    std::vector<VertexAttributes> m_vertex_attributes;
    std::vector<uint32_t> m_indices;
    VkBuffer m_vertex_buffer;
    VkBuffer m_index_buffer;
    VmaAllocation m_vb_allocation;
    VmaAllocation m_ib_allocation;
};
} // namespace rendersystem