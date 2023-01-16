#pragma once

#include "VkBootstrap.h"
#include "core.h"
#include "mesh.h"
#include "pass.h"
#include "swapchain.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

class Engine
{
  private:
    engine::CoreData m_core;
    engine::SwapChainData m_swapchain;
    engine::PassData m_pass;

    VkPipeline m_pipeline;
    VkPipelineLayout m_pipeline_layout;
    VkShaderModule m_triangle_frag;
    VkShaderModule m_triangle_vert;

  private:
    void init_pipelines();

  public:
    Engine();
    void init(uint32_t width, uint32_t height);
    void cleanup();
    void draw(size_t frame_number);
    void run();
};