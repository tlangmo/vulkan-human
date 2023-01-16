#include "mesh.h"
#include "check.h"
#include <cstring>
#include <vk_mem_alloc.h>

namespace engine
{

std::vector<VertexAttributes>& Mesh::vertices()
{
    return m_vertex_attributes;
}
const std::vector<VertexAttributes>& Mesh::vertices() const
{
    return m_vertex_attributes;
}

void Mesh::create(VmaAllocator vma_allocator)
{
    assert(m_buffer == nullptr);
    if (m_vertex_attributes.empty())
    {
        throw std::runtime_error("cannot upload an empty mesh");
    }

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    // this is the total size, in bytes, of the buffer we are allocating
    bufferInfo.size = m_vertex_attributes.size() * sizeof(VertexAttributes);
    // this buffer is going to be used as a Vertex Buffer
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    // let the VMA library know that this data should be writeable by CPU, but also readable by GPU
    VmaAllocationCreateInfo vmaallocInfo = {};
    vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    // allocate the buffer
    VK_CHECK_RESULT(vmaCreateBuffer(vma_allocator, &bufferInfo, &vmaallocInfo, &m_buffer, &m_allocation, nullptr));

    void* data;
    VK_CHECK_RESULT(vmaMapMemory(vma_allocator, m_allocation, &data));
    memcpy(data, m_vertex_attributes.data(), m_vertex_attributes.size() * sizeof(VertexAttributes));
    vmaUnmapMemory(vma_allocator, m_allocation);
}

void Mesh::destroy(VmaAllocator vma_allocator)
{
    vmaDestroyBuffer(vma_allocator, m_buffer, m_allocation);
}

static VertexInputDescription get_input_desc()
{
    // we will have just 1 vertex buffer binding, with a per-vertex rate
    VertexInputDescription description;
    VkVertexInputBindingDescription mainBinding = {};
    mainBinding.binding = 0;
    mainBinding.stride = sizeof(VertexAttributes);
    mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    description.bindings.push_back(mainBinding);

    // Position will be stored at Location 0
    VkVertexInputAttributeDescription positionAttribute = {};
    positionAttribute.binding = 0;
    positionAttribute.location = 0;
    positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    positionAttribute.offset = offsetof(VertexAttributes, position);

    // Normal will be stored at Location 1
    VkVertexInputAttributeDescription normalAttribute = {};
    normalAttribute.binding = 0;
    normalAttribute.location = 1;
    normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    normalAttribute.offset = offsetof(VertexAttributes, normal);

    // Color will be stored at Location 2
    VkVertexInputAttributeDescription colorAttribute = {};
    colorAttribute.binding = 0;
    colorAttribute.location = 2;
    colorAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    colorAttribute.offset = offsetof(VertexAttributes, color);

    description.attributes.push_back(positionAttribute);
    description.attributes.push_back(normalAttribute);
    description.attributes.push_back(colorAttribute);
    return description;
}
VertexInputDescription& Mesh::get_vertex_input_description()
{
    static VertexInputDescription description = get_input_desc();
    return description;
}
} // namespace engine