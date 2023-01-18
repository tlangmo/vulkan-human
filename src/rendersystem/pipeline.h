#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace rendersystem
{

/**
 * Mechanism to build VkPipeline objects more conveniently
 */
class PipelineBuilder
{

  public:
    PipelineBuilder& add_shader_stage(const VkPipelineShaderStageCreateInfo& info);
    PipelineBuilder& add_vertex_input_state(const VkPipelineVertexInputStateCreateInfo& info);
    PipelineBuilder& add_input_assembly_state(const VkPipelineInputAssemblyStateCreateInfo& info);
    PipelineBuilder& add_rasterization_state(const VkPipelineRasterizationStateCreateInfo& info);
    PipelineBuilder& depth_stencil(bool enabled, bool write_depth, VkCompareOp compare_op);
    PipelineBuilder& add_viewport(const VkViewport& view);
    // do not use any multisampling anti-aliasign
    PipelineBuilder& no_msaa();
    PipelineBuilder& no_color_blend();
    /**
     * Use all previous information an create the vulkan pipeline. Note that
     */
    VkPipeline build(VkDevice device, VkRenderPass pass, VkPipelineLayout layout);

  private:
    std::vector<VkPipelineShaderStageCreateInfo> m_shader_stages;
    VkPipelineVertexInputStateCreateInfo m_vertex_input_state;
    VkPipelineInputAssemblyStateCreateInfo m_input_assembly_state;
    VkPipelineRasterizationStateCreateInfo m_rasterization_state;
    VkPipelineMultisampleStateCreateInfo m_multisample_state;
    VkPipelineColorBlendAttachmentState m_color_blend_attachement_state;
    VkPipelineColorBlendStateCreateInfo m_color_blend_state;
    VkPipelineDepthStencilStateCreateInfo m_depth_stencil_state;
    VkViewport m_viewport;
    VkRect2D m_scissor;
    VkPipelineLayout m_pipeline_layout;
};

void load_shader_module(VkDevice device, const char* file_path, VkShaderModule* out_shader_module);
} // namespace rendersystem