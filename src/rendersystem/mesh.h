#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include <vulkan/vulkan.h>

// forward decl
VK_DEFINE_HANDLE(VmaAllocation)
VK_DEFINE_HANDLE(VmaAllocator)

namespace engine
{

struct VertexAttributes
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
};

struct VertexInputDescription
{
    std::vector<VkVertexInputBindingDescription> bindings;
    std::vector<VkVertexInputAttributeDescription> attributes;
    VkPipelineVertexInputStateCreateFlags flags = 0;
};

class Mesh
{
  public:
    std::vector<VertexAttributes>& vertices();
    const std::vector<VertexAttributes>& vertices() const;
    static VertexInputDescription& get_vertex_input_description();
    void create(VmaAllocator vma_allocator);
    void destroy(VmaAllocator vma_allocator);

    std::vector<VertexAttributes> m_vertex_attributes;
    VkBuffer m_buffer;
    VmaAllocation m_allocation;
};
} // namespace engine