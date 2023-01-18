
#include "rendersystem.h"
#include "pipeline.h"
#define GLFW_INCLUDE_VULKAN
#include "VkBootstrap.h"
#include "check.h"
#include "components/coordsys.h"
#include "components/visual.h"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <vector>
namespace rendersystem
{

struct MeshPushConstants
{
    glm::vec4 data;
    glm::mat4 render_matrix;
};

RenderSystem::RenderSystem()
{
}

void RenderSystem::create(uint32_t width, uint32_t height)
{
    m_core = rendersystem::create_core_with_window("vulkan_human", 640, 480);
    m_swapchain = rendersystem::create_swapchain(m_core);
    m_pass = rendersystem::create_basic_pass(m_core, m_swapchain);

    create_pipeline();
}

void RenderSystem::destroy()
{
    vkDeviceWaitIdle(m_core.device);
    vkWaitForFences(m_core.device, 1, &m_core.fence_host, VK_TRUE, UINT64_MAX);

    for (auto& entry : m_meshes)
    {
        entry.second->destroy(m_core.allocator);
    }
    vkDestroyPipeline(m_core.device, m_pipeline, nullptr);
    vkDestroyPipelineLayout(m_core.device, m_pipeline_layout, nullptr);
    vkDestroyShaderModule(m_core.device, m_triangle_frag, nullptr);
    vkDestroyShaderModule(m_core.device, m_triangle_vert, nullptr);

    rendersystem::destroy_pass(m_core.device, &m_pass);
    rendersystem::destroy_swapchain(m_core, &m_swapchain);
    rendersystem::destroy_core(&m_core);
}

void RenderSystem::create_pipeline()
{
    rendersystem::load_shader_module(m_core.device, "rendersystem/shaders/mesh.vert.spv", &m_triangle_vert);
    rendersystem::load_shader_module(m_core.device, "rendersystem/shaders/mesh.frag.spv", &m_triangle_frag);

    rendersystem::PipelineBuilder builder;
    builder.add_shader_stage(
        VkPipelineShaderStageCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                        .pNext = nullptr,
                                        .flags = {},
                                        .stage = VK_SHADER_STAGE_VERTEX_BIT,
                                        .module = m_triangle_vert,
                                        .pName = "main"});
    builder.add_shader_stage(
        VkPipelineShaderStageCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                        .pNext = nullptr,
                                        .flags = {},
                                        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                                        .module = m_triangle_frag,
                                        .pName = "main"});

    builder.add_vertex_input_state(VkPipelineVertexInputStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .flags = VkPipelineVertexInputStateCreateFlags{},
        .vertexBindingDescriptionCount = (uint32_t)Mesh::get_vertex_input_description().bindings.size(),
        .pVertexBindingDescriptions = Mesh::get_vertex_input_description().bindings.data(),
        .vertexAttributeDescriptionCount = (uint32_t)Mesh::get_vertex_input_description().attributes.size(),
        .pVertexAttributeDescriptions = Mesh::get_vertex_input_description().attributes.data()});
    builder.add_input_assembly_state(
        VkPipelineInputAssemblyStateCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                                               .topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST});

    builder.add_rasterization_state(
        VkPipelineRasterizationStateCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                                               .depthClampEnable = VK_FALSE,
                                               .rasterizerDiscardEnable = VK_FALSE,
                                               .polygonMode = VK_POLYGON_MODE_FILL,
                                               .cullMode = VK_CULL_MODE_NONE,
                                               .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                               .depthBiasEnable = VK_FALSE,
                                               .depthBiasConstantFactor = 0.0f,
                                               .depthBiasClamp = 0.0f,
                                               .depthBiasSlopeFactor = 0.0f,
                                               .lineWidth = 1.0f});
    builder.add_viewport({.x = 0,
                          .y = 0,
                          .width = (float)m_core.window_size.width,
                          .height = (float)m_core.window_size.height,
                          .minDepth = 0,
                          .maxDepth = 1});
    builder.depth_stencil(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);
    builder.no_msaa();
    builder.no_color_blend();

    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.pNext = nullptr;
    // empty defaults
    pipeline_layout_info.flags = 0;
    pipeline_layout_info.setLayoutCount = 0;
    pipeline_layout_info.pSetLayouts = nullptr;

    // setup push constants
    VkPushConstantRange push_constant;
    // this push constant range starts at the beginning
    push_constant.offset = 0;
    // this push constant range takes up the size of a MeshPushConstants struct
    push_constant.size = sizeof(MeshPushConstants);
    // this push constant range is accessible only in the vertex shader
    push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    pipeline_layout_info.pPushConstantRanges = &push_constant;
    pipeline_layout_info.pushConstantRangeCount = 1;

    VK_CHECK_RESULT(vkCreatePipelineLayout(m_core.device, &pipeline_layout_info, nullptr, &m_pipeline_layout));

    m_pipeline = builder.build(m_core.device, m_pass.render_pass, m_pipeline_layout);
}

void RenderSystem::draw(const std::vector<Entity>& entities, size_t frame_number)
{
    /*Wait for the previous frame to finish
        Acquire an image from the swap chain
        Record a command buffer which draws the scene onto that image
        Submit the recorded command buffer
        Present the swap chain image*/
    const uint64_t kTimeout = 1'000'000'000;
    uint32_t swap_chain_index = 0;
    vkWaitForFences(m_core.device, 1, &m_core.fence_host, VK_TRUE, UINT64_MAX);
    VK_CHECK_RESULT(vkAcquireNextImageKHR(m_core.device, m_swapchain.swapchain_khr, kTimeout, m_core.semaphore_present,
                                          nullptr, &swap_chain_index));

    // // https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkFence.html
    // // A fence can be signaled as part of the execution of a queue submission command.
    // // Fences can be unsignaled on the host with vkResetFences.

    vkResetCommandBuffer(m_core.cmd_buf_main, 0);

    VkCommandBufferBeginInfo cmd_begin_info = {};
    cmd_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VK_CHECK_RESULT(vkBeginCommandBuffer(m_core.cmd_buf_main, &cmd_begin_info));

    VkClearValue clearValue;
    float flash = std::abs(sin((double)frame_number / 30.f));
    clearValue.color = {{0.0f, 1.0f, flash, 1.0f}};

    VkClearValue depthClear;
    depthClear.depthStencil.depth = 1.f;

    VkClearValue clearValues[] = {clearValue, depthClear};
    VkRenderPassBeginInfo rp_info = {};
    rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_info.pNext = nullptr;
    rp_info.renderPass = m_pass.render_pass;
    rp_info.renderArea.offset.x = 0;
    rp_info.renderArea.offset.y = 0;
    rp_info.renderArea.extent = m_core.window_size;
    rp_info.framebuffer = m_pass.frame_buffers[swap_chain_index];
    rp_info.clearValueCount = 2;
    rp_info.pClearValues = clearValues;

    vkCmdBeginRenderPass(m_core.cmd_buf_main, &rp_info, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(m_core.cmd_buf_main, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
    // bind the mesh vertex buffer with offset 0
    VkDeviceSize offset = 0;
    for (const auto& ent : entities)
    {
        auto coord = ent.get_component<CoordSysComponent>();
        auto viz = ent.get_component<VisualComponent>();
        if (!viz)
        {
            continue;
        }
        auto& render_mesh = m_meshes[viz->hash()];
        MeshPushConstants constants;
        glm::vec3 cam_pos = {0.f, 0.f, -2.f};
        glm::mat4 view = glm::translate(glm::mat4(1.f), cam_pos);
        // camera projection
        glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
        projection[1][1] *= -1;
        // model rotation
        glm::mat4 model = glm::rotate(glm::mat4{1.0f}, glm::radians(frame_number * 0.4f), glm::vec3(0, 1, 0));
        if (coord)
        {
            model = coord->mat_world() * model;
        }

        // calculate final mesh matrix
        glm::mat4 mesh_matrix = projection * view * model;
        constants.render_matrix = mesh_matrix;
        // constants.render_matrix =glm::mat4{1.0};
        ;
        // upload the matrix to the GPU via push constants
        vkCmdPushConstants(m_core.cmd_buf_main, m_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0,
                           sizeof(MeshPushConstants), &constants);

        vkCmdBindVertexBuffers(m_core.cmd_buf_main, 0, 1, &render_mesh->vb(), &offset);
        vkCmdBindIndexBuffer(m_core.cmd_buf_main, render_mesh->ib(), 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(m_core.cmd_buf_main, render_mesh->indices().size(), 1, 0, 0, 0);
        // vkCmdDraw(m_core.cmd_buf_main, render_mesh->vertices().size(), 1, 0, 0);
    }

    vkCmdEndRenderPass(m_core.cmd_buf_main);
    VK_CHECK_RESULT(vkEndCommandBuffer(m_core.cmd_buf_main));

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pNext = nullptr;

    VkSemaphore wait_semaphores[] = {m_core.semaphore_present};
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;

    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &m_core.semaphore_render;

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &m_core.cmd_buf_main;

    vkResetFences(m_core.device, 1, &m_core.fence_host);

    vkQueueSubmit(m_core.graphics_queue, 1, &submit_info, m_core.fence_host);

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = NULL;
    VkSwapchainKHR swapChains[] = {m_swapchain.swapchain_khr};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &swap_chain_index;

    presentInfo.pWaitSemaphores = &m_core.semaphore_render;
    presentInfo.waitSemaphoreCount = 1;

    VkResult res = vkQueuePresentKHR(m_core.present_queue, &presentInfo);
}

void RenderSystem::process(const std::vector<Entity>& entities)
{
    size_t frame_number = 0;
    while (!glfwWindowShouldClose(m_core.window))
    {
        for (auto e : entities)
        {
            if (auto viz = e.get_component<VisualComponent>(); viz != nullptr)
            {
                // std::size_t viz_com_hash = std::hash<std::shared_ptr<VisualComponent>>{}(viz);
                size_t viz_com_hash = viz->hash();
                if (m_meshes.find(viz_com_hash) == m_meshes.end())
                {
                    m_meshes[viz_com_hash] = create_mesh_from_vertex_data(viz->vertices(), viz->indices());
                    m_meshes[viz_com_hash]->create(m_core.allocator);
                }
            }
        }
        if (glfwGetKey(m_core.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(m_core.window, true);
        }
        if (glfwGetMouseButton(m_core.window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
            std::cout << "right mouse button pressed" << std::endl;
        }

        glfwPollEvents();
        draw(entities, frame_number);
        frame_number++;
    }
    glfwDestroyWindow(m_core.window);
    glfwTerminate();
}

} // namespace rendersystem