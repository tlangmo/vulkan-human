#include "pipeline.h"
#include <fstream>
#include <iostream>

namespace engine
{

PipelineBuilder& PipelineBuilder::add_shader_stage(const VkPipelineShaderStageCreateInfo& info)
{
    m_shader_stages.push_back(info);
    return *this;
}

PipelineBuilder& PipelineBuilder::add_vertex_input_state(const VkPipelineVertexInputStateCreateInfo& info)
{
    m_vertex_input_state = info;
    return *this;
}

PipelineBuilder& PipelineBuilder::add_input_assembly_state(const VkPipelineInputAssemblyStateCreateInfo& info)
{
    m_input_assembly_state = info;
    return *this;
}

PipelineBuilder& PipelineBuilder::add_rasterization_state(const VkPipelineRasterizationStateCreateInfo& info)
{
    m_rasterization_state = info;
    return *this;
}
PipelineBuilder& PipelineBuilder::no_msaa()
{
    VkPipelineMultisampleStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    info.pNext = nullptr;

    info.sampleShadingEnable = VK_FALSE;
    // multisampling defaulted to no multisampling (1 sample per pixel)
    info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    info.minSampleShading = 1.0f;
    info.pSampleMask = nullptr;
    info.alphaToCoverageEnable = VK_FALSE;
    info.alphaToOneEnable = VK_FALSE;
    m_multisample_state = info;
    return *this;
}

PipelineBuilder& PipelineBuilder::no_color_blend()
{
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    m_color_blend_attachement_state = colorBlendAttachment;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.pNext = nullptr;

    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &m_color_blend_attachement_state;
    m_color_blend_state = colorBlending;
    return *this;
}

PipelineBuilder& PipelineBuilder::add_viewport(const VkViewport& view)
{
    m_viewport = view;
    m_scissor.offset = {0, 0};
    m_scissor.extent =
        VkExtent2D{.width = static_cast<uint32_t>(view.width), .height = static_cast<uint32_t>(view.height)};
    return *this;
}

VkPipeline PipelineBuilder::build(VkDevice device, VkRenderPass pass, VkPipelineLayout layout)
{
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &m_viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &m_scissor;

    VkGraphicsPipelineCreateInfo pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.pNext = nullptr;

    pipeline_info.stageCount = m_shader_stages.size();
    pipeline_info.pStages = m_shader_stages.data();
    pipeline_info.pVertexInputState = &m_vertex_input_state;
    pipeline_info.pInputAssemblyState = &m_input_assembly_state;
    pipeline_info.pViewportState = &viewportState;
    pipeline_info.pRasterizationState = &m_rasterization_state;
    pipeline_info.pMultisampleState = &m_multisample_state;
    pipeline_info.pColorBlendState = &m_color_blend_state;
    pipeline_info.layout = layout;
    pipeline_info.renderPass = pass;
    pipeline_info.subpass = 0;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

    // //it's easy to error out on create graphics pipeline, so we handle it a bit better than the common VK_CHECK case
    VkPipeline newPipeline;
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &newPipeline) != VK_SUCCESS)
    {
        std::cout << "failed to create pipeline\n";
        return VK_NULL_HANDLE; // failed to create graphics pipeline
    }
    else
    {
        return newPipeline;
    }
}

void load_shader_module(VkDevice device, const char* filePath, VkShaderModule* out_shader_module)
{
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);
    if (!file.is_open())
    {
        char msg[128];
        snprintf(msg, sizeof(msg), "file not found: %s", filePath);
        throw std::runtime_error(msg);
    }
    size_t file_size = (size_t)file.tellg();
    // spirv expects the buffer to be on uint32 aligned, so make sure to reserve an int vector big enough for the entire
    // file
    std::vector<uint32_t> buffer(file_size / sizeof(uint32_t));
    // put file cursor at beginning
    file.seekg(0);

    // load the entire file into the buffer
    file.read((char*)buffer.data(), file_size);
    file.close();

    VkShaderModuleCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = buffer.size() * sizeof(u_int32_t);
    create_info.pCode = buffer.data();
    VkShaderModule shader_module;
    if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS)
    {
        throw std::runtime_error("invalid shader file");
    }
    *out_shader_module = shader_module;
}
} // namespace engine