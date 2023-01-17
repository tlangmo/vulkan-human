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
    Mesh() : m_vertex_attributes(), m_buffer(), m_allocation()
    {
    }
    Mesh(const tinygltf::Model& model);
    Mesh(const Mesh& rhs) = delete;
    Mesh(const Mesh&& rhs) = delete;
    std::vector<VertexAttributes>& vertices();
    const std::vector<VertexAttributes>& vertices() const;
    VkBuffer& buffer()
    {
        return m_buffer;
    }
    void create(VmaAllocator vma_allocator);
    void destroy(VmaAllocator vma_allocator);

  public:
    static VertexInputDescriptionData& get_vertex_input_description();

  private:
    std::vector<VertexAttributes> m_vertex_attributes;
    VkBuffer m_buffer;
    VmaAllocation m_allocation;
};
} // namespace rendersystem