
#include "rendersystem.h"
#include "pipeline.h"
#define GLFW_INCLUDE_VULKAN
#include "VkBootstrap.h"
#include "check.h"
#include "components/coordsys.h"
#include "components/visual.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <vector>
namespace engine
{
RenderSystem::RenderSystem()
{
}

void RenderSystem::create(uint32_t width, uint32_t height)
{
    m_core = engine::create_core_with_window("vulkan_human", 640, 480);
    m_swapchain = engine::create_swapchain(m_core.physical_device, m_core.device, m_core.surface);
    m_pass = engine::create_simple_pass(m_core, m_swapchain);

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

    engine::destroy_data_object(m_pass);
    engine::destroy_data_object(m_swapchain);
    engine::destroy_data_object(m_core);
}

void RenderSystem::create_pipeline()
{
    engine::load_shader_module(m_core.device, "shaders/triangle.vert.spv", &m_triangle_vert);
    engine::load_shader_module(m_core.device, "shaders/triangle.frag.spv", &m_triangle_frag);

    engine::PipelineBuilder builder;
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
    auto he = Mesh::get_vertex_input_description();
    std::vector<VkVertexInputBindingDescription>& bindings = Mesh::get_vertex_input_description().bindings;
    std::vector<VkVertexInputAttributeDescription>& attribs = Mesh::get_vertex_input_description().attributes;
    builder.add_vertex_input_state(
        VkPipelineVertexInputStateCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                                             .flags = VkPipelineVertexInputStateCreateFlags{},
                                             .vertexBindingDescriptionCount = (uint32_t)bindings.size(),
                                             .pVertexBindingDescriptions = bindings.data(),
                                             .vertexAttributeDescriptionCount = (uint32_t)attribs.size(),
                                             .pVertexAttributeDescriptions = attribs.data()});
    builder.add_input_assembly_state(
        VkPipelineInputAssemblyStateCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                                               .topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST});

    builder.add_rasterization_state(
        VkPipelineRasterizationStateCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                                               .depthClampEnable = VK_FALSE,
                                               .rasterizerDiscardEnable = VK_FALSE,
                                               .polygonMode = VK_POLYGON_MODE_FILL,
                                               .cullMode = VK_CULL_MODE_NONE,
                                               .frontFace = VK_FRONT_FACE_CLOCKWISE,
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
    builder.no_msaa();
    builder.no_color_blend();

    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.pNext = nullptr;
    // empty defaults
    pipeline_layout_info.flags = 0;
    pipeline_layout_info.setLayoutCount = 0;
    pipeline_layout_info.pSetLayouts = nullptr;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;
    VK_CHECK_RESULT(vkCreatePipelineLayout(m_core.device, &pipeline_layout_info, nullptr, &m_pipeline_layout));

    m_pipeline = builder.build(m_core.device, m_pass.render_pass, m_pipeline_layout);
}

void RenderSystem::draw(size_t frame_number)
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

    VkRenderPassBeginInfo rp_info = {};
    rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_info.pNext = nullptr;
    rp_info.renderPass = m_pass.render_pass;
    rp_info.renderArea.offset.x = 0;
    rp_info.renderArea.offset.y = 0;
    rp_info.renderArea.extent = m_core.window_size;
    rp_info.framebuffer = m_pass.frame_buffers[swap_chain_index];
    rp_info.clearValueCount = 1;
    rp_info.pClearValues = &clearValue;

    vkCmdBeginRenderPass(m_core.cmd_buf_main, &rp_info, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(m_core.cmd_buf_main, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
    // bind the mesh vertex buffer with offset 0
    VkDeviceSize offset = 0;
    for (const auto& val : m_meshes)
    {
        vkCmdBindVertexBuffers(m_core.cmd_buf_main, 0, 1, &val.second->m_buffer, &offset);
        vkCmdDraw(m_core.cmd_buf_main, val.second->m_vertex_attributes.size(), 1, 0, 0);
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
            auto viz = e.get_component<VisualComponent>();
            if (viz == nullptr)
            {
                std::cout << "not a real one" << std::endl;
            }

            if (viz != nullptr)
            {
                std::size_t viz_com_hash = std::hash<std::shared_ptr<VisualComponent>>{}(viz);
                if (m_meshes.find(viz_com_hash) == m_meshes.end())
                {
                    auto new_mesh = std::make_unique<Mesh>();
                    std::transform(viz->vertices().begin(), viz->vertices().end(),
                                   std::back_inserter(new_mesh->vertices()), [](ColoredVertex& v) -> VertexAttributes {
                                       VertexAttributes va;
                                       va.position[0] = v.position[0];
                                       va.position[1] = v.position[1];
                                       va.position[2] = v.position[2];

                                       va.color[0] = v.color[0];
                                       va.color[1] = v.color[1];
                                       va.color[2] = v.color[2];
                                       return va;
                                   });
                    new_mesh->create(m_core.allocator);
                    m_meshes[viz_com_hash] = std::move(new_mesh);
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
        draw(frame_number);
        frame_number++;
    }
    glfwDestroyWindow(m_core.window);
    glfwTerminate();
}

} // namespace engine