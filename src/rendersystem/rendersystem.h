#pragma once

#include "VkBootstrap.h"
#include "camera.h"
#include "core.h"
#include "entity.h"
#include "mesh.h"
#include "pass.h"
#include "swapchain.h"
#include <algorithm>
#include <chrono>
#include <memory>
#include <mesh.h>
#include <vector>
#include <vulkan/vulkan.h>
namespace rendersystem
{

template <typename T>
std::unique_ptr<Mesh> create_mesh_from_vertex_data(const std::vector<T>& vertices, const std::vector<uint32_t>& indices)
{
    auto render_mesh = std::make_unique<Mesh>();
    std::transform(vertices.begin(), vertices.end(), std::back_inserter(render_mesh->vertices()),
                   [](const T& v) -> VertexAttributes {
                       VertexAttributes va = {};
                       va.position[0] = v.position[0];
                       va.position[1] = v.position[1];
                       va.position[2] = v.position[2];

                       va.normal[0] = v.normal[0];
                       va.normal[1] = v.normal[1];
                       va.normal[2] = v.normal[2];

                       va.color[0] = v.color[0];
                       va.color[1] = v.color[1];
                       va.color[2] = v.color[2];
                       return va;
                   });
    render_mesh->indices() = indices;
    return std::move(render_mesh);
}

class RenderSystem
{
  public:
    RenderSystem();
    GLFWwindow* create(uint32_t width, uint32_t height);
    void destroy();
    void process(const std::vector<components::Entity>& entities, uint64_t elapsed_us);

  private:
    void create_pipeline();
    void draw(components::Entity* entity, uint64_t elapsed_us, std::shared_ptr<components::Camera> camera);
    uint32_t begin_pass(VkClearColorValue clear_color);
    void present_pass(uint32_t swap_chain_index);

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