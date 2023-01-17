#include "mesh.h"
#include "check.h"
#include <cstring>
#include <vk_mem_alloc.h>
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

namespace rendersystem
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

static VertexInputDescriptionData get_input_desc()
{
    // we will have just 1 vertex buffer binding, with a per-vertex rate
    VkVertexInputBindingDescription main_binding = {};
    main_binding.binding = 0;
    main_binding.stride = sizeof(VertexAttributes);
    main_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // Position will be stored at Location 0
    VkVertexInputAttributeDescription position_attribute = {};
    position_attribute.binding = 0;
    position_attribute.location = 0;
    position_attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    position_attribute.offset = offsetof(VertexAttributes, position);

    // Normal will be stored at Location 1
    VkVertexInputAttributeDescription normal_attribute = {};
    normal_attribute.binding = 0;
    normal_attribute.location = 1;
    normal_attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    normal_attribute.offset = offsetof(VertexAttributes, normal);

    // Color will be stored at Location 2
    VkVertexInputAttributeDescription color_attribute = {};
    color_attribute.binding = 0;
    color_attribute.location = 2;
    color_attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    color_attribute.offset = offsetof(VertexAttributes, color);

    return VertexInputDescriptionData{
        .bindings{main_binding}, .attributes{position_attribute, normal_attribute, color_attribute}, .flags{}};
}
VertexInputDescriptionData& Mesh::get_vertex_input_description()
{
    static VertexInputDescriptionData description = get_input_desc();
    return description;
}

} // namespace rendersystem