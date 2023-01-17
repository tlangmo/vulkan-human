#pragma once

#include "VkBootstrap.h"
#include "core.h"
#include "entity.h"
#include "mesh.h"
#include "pass.h"
#include "swapchain.h"
#include <memory>
#include <mesh.h>
#include <vector>
#include <vulkan/vulkan.h>
namespace rendersystem
{
class RenderSystem
{
  public:
    RenderSystem();
    void create(uint32_t width, uint32_t height);
    void destroy();
    void process(const std::vector<Entity>& entities);

  private:
    void create_pipeline();
    void draw(const std::vector<Entity>& entities, size_t frame_number);

  private:
    rendersystem::CoreData m_core;
    rendersystem::SwapChainData m_swapchain;
    rendersystem::PassData m_pass;

    VkPipeline m_pipeline;
    VkPipelineLayout m_pipeline_layout;
    VkShaderModule m_triangle_frag;
    VkShaderModule m_triangle_vert;

    std::unordered_map<std::size_t, std::unique_ptr<Mesh>> m_meshes;
};
} // namespace rendersystem