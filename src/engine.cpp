
#include "engine.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VkBootstrap.h"
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <iostream>

std::string errorString(VkResult errorCode)
{
    switch (errorCode)
    {
#define STR(r)                                                                                                         \
    case VK_##r:                                                                                                       \
        return #r
        STR(NOT_READY);
        STR(TIMEOUT);
        STR(EVENT_SET);
        STR(EVENT_RESET);
        STR(INCOMPLETE);
        STR(ERROR_OUT_OF_HOST_MEMORY);
        STR(ERROR_OUT_OF_DEVICE_MEMORY);
        STR(ERROR_INITIALIZATION_FAILED);
        STR(ERROR_DEVICE_LOST);
        STR(ERROR_MEMORY_MAP_FAILED);
        STR(ERROR_LAYER_NOT_PRESENT);
        STR(ERROR_EXTENSION_NOT_PRESENT);
        STR(ERROR_FEATURE_NOT_PRESENT);
        STR(ERROR_INCOMPATIBLE_DRIVER);
        STR(ERROR_TOO_MANY_OBJECTS);
        STR(ERROR_FORMAT_NOT_SUPPORTED);
        STR(ERROR_SURFACE_LOST_KHR);
        STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
        STR(SUBOPTIMAL_KHR);
        STR(ERROR_OUT_OF_DATE_KHR);
        STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
        STR(ERROR_VALIDATION_FAILED_EXT);
        STR(ERROR_INVALID_SHADER_NV);
#undef STR
    default:
        return "UNKNOWN_ERROR";
    }
}

#define VK_CHECK_RESULT(f)                                                                                             \
    {                                                                                                                  \
        VkResult res = (f);                                                                                            \
        if (res != VK_SUCCESS)                                                                                         \
        {                                                                                                              \
            std::cout << "Fatal : VkResult is \"" << errorString(res) << "\" in " << __FILE__ << " at line "           \
                      << __LINE__ << "\n";                                                                             \
            assert(res == VK_SUCCESS);                                                                                 \
        }                                                                                                              \
    }

Engine::Engine() : m_swapchain(nullptr)
{
}

bool isDeviceSuitable(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
}

void Engine::init(uint32_t width, uint32_t height)
{
    // We initialize SDL and create a window with it.

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(width, height, "Vulkan window", nullptr, nullptr);
    m_window_size = VkExtent2D{width, height};
    uint32_t ext_count = 0;
    const char** glfw_extensions;

    glfw_extensions = glfwGetRequiredInstanceExtensions(&ext_count);

    vkb::InstanceBuilder builder;
    builder.set_app_name("Vulkan Human")
        .request_validation_layers(true)
        .use_default_debug_messenger()
        .require_api_version(1, 1, 0);
    for (int i = 0; i < ext_count; i++)
    {
        builder.enable_extension(glfw_extensions[i]);
    }

    vkb::Instance vkb_instance = builder.build().value();
    m_instance = vkb_instance.instance;
    m_debug_messenger = vkb_instance.debug_messenger;

    if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }

    // use vkbootstrap to select a GPU.
    vkb::PhysicalDeviceSelector selector{vkb_instance};
    vkb::PhysicalDevice vkb_physical_device = selector.set_minimum_version(1, 2)
                                                  .set_surface(m_surface)
                                                  //   .prefer_gpu_device_type()
                                                  //   .require_present(true)
                                                  .select()
                                                  .value();

    vkb::DeviceBuilder vkb_device_builder{vkb_physical_device};
    vkb::Device vkb_device = vkb_device_builder.build().value();

    // Get the VkDevice handle used in the rest of a Vulkan application
    m_device = vkb_device.device;
    m_physical_device = vkb_physical_device.physical_device;
    m_graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
    m_graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

    m_present_queue = vkb_device.get_queue(vkb::QueueType::present).value();
    m_present_queue_family = vkb_device.get_queue_index(vkb::QueueType::present).value();

    init_swapchain(vkb_device);
    init_commands();
    init_default_renderpass();
    init_framebuffers();
    init_barriers();
}

void Engine::init_swapchain(vkb::Device dev)
{
    // vkb::SwapchainBuilder vkb_swapchain_builder(m_physical_device, m_device, m_surface,0,0);
    vkb::SwapchainBuilder vkb_swapchain_builder(dev);

    vkb::Swapchain vkb_swapchain = vkb_swapchain_builder
                                       .use_default_format_selection()
                                       // use vsync present mode
                                       //    .set_desired_present_mode(VK_PRESENT_MODE_IMMEDIATE_KHR)
                                       //    .set_desired_extent(m_window_size.width, m_window_size.height)
                                       //    .set_desired_min_image_count(1)
                                       .build()
                                       .value();
    // https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Physical_devices_and_queue_families
    m_swapchain = vkb_swapchain.swapchain;
    m_swapchain_images = vkb_swapchain.get_images().value();
    m_swapchain_image_views = vkb_swapchain.get_image_views().value();
    m_swapchain_format = vkb_swapchain.image_format;
}

void Engine::init_commands()
{
    // first the pool
    VkCommandPoolCreateInfo cmd_pool_info = {};
    cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmd_pool_info.pNext = nullptr;
    cmd_pool_info.queueFamilyIndex = m_graphics_queue_family;
    cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    vkCreateCommandPool(m_device, &cmd_pool_info, nullptr, &m_command_pool);

    // then the buffer
    VkCommandBufferAllocateInfo cmd_buf_info = {};
    cmd_buf_info.pNext = nullptr;
    cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd_buf_info.commandPool = m_command_pool;
    cmd_buf_info.commandBufferCount = 1;
    cmd_buf_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    vkAllocateCommandBuffers(m_device, &cmd_buf_info, &m_main_command_buffer);
}

void Engine::init_default_renderpass()
{
    // a nice explanation: https://developer.samsung.com/galaxy-gamedev/resources/articles/renderpasses.html
    VkAttachmentDescription color_attachment = {};
    color_attachment.format = m_swapchain_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = {};
    // attachment number will index into the pAttachments array in the parent renderpass itself
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.pNext = nullptr;
    // connect the color attachment to the info
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    // connect the subpass to the info
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    // render_pass_info.dependencyCount = 1;
    // render_pass_info.pDependencies = &dependency;

    vkCreateRenderPass(m_device, &render_pass_info, nullptr, &m_render_pass);
}

void Engine::init_framebuffers()
{
    VkFramebufferCreateInfo fb_info = {};
    fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_info.pNext = nullptr;

    fb_info.renderPass = m_render_pass;
    fb_info.attachmentCount = 1;
    fb_info.width = m_window_size.width;
    fb_info.height = m_window_size.height;
    fb_info.layers = 1;

    m_frame_buffers.clear();

    std::transform(m_swapchain_image_views.begin(), m_swapchain_image_views.end(), std::back_inserter(m_frame_buffers),
                   [&](const VkImageView img) -> VkFramebuffer {
                       fb_info.pAttachments = &img;
                       VkFramebuffer fb;
                       VK_CHECK_RESULT(vkCreateFramebuffer(m_device, &fb_info, nullptr, &fb));
                       return fb;
                   });
}
void Engine::init_barriers()
{
    VkFenceCreateInfo fence_create_info = {};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    vkCreateFence(m_device, &fence_create_info, nullptr, &m_fence_render);

    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VK_CHECK_RESULT(vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_semaphore_present));
    VK_CHECK_RESULT(vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_semaphore_render));
}

void Engine::cleanup()
{
    vkWaitForFences(m_device, 1, &m_fence_render, VK_TRUE, UINT64_MAX);
    if (m_fence_render)
    {
        vkDestroyFence(m_device, m_fence_render, nullptr);
    }
    if (m_semaphore_present)
    {
        vkDestroySemaphore(m_device, m_semaphore_present, nullptr);
        vkDestroySemaphore(m_device, m_semaphore_render, nullptr);
    }

    if (m_render_pass)
    {
        vkDestroyRenderPass(m_device, m_render_pass, nullptr);
    }
    if (m_command_pool)
    {
        vkDestroyCommandPool(m_device, m_command_pool, nullptr);
    }
    if (m_swapchain)
    {
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
        for (auto sc : m_swapchain_image_views)
        {
            vkDestroyImageView(m_device, sc, nullptr);
        }
        for (auto fb : m_frame_buffers)
        {
            vkDestroyFramebuffer(m_device, fb, nullptr);
        }
    }
    vkDestroyDevice(m_device, nullptr);
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);
    vkDestroyInstance(m_instance, nullptr);
}

void Engine::draw(size_t frame_number)
{
    /*Wait for the previous frame to finish
Acquire an image from the swap chain
Record a command buffer which draws the scene onto that image
Submit the recorded command buffer
Present the swap chain image*/
    const uint64_t kTimeout = 1000000000;
    uint32_t swap_chain_index = 20;
    vkWaitForFences(m_device, 1, &m_fence_render, VK_TRUE, UINT64_MAX);
    VK_CHECK_RESULT(
        vkAcquireNextImageKHR(m_device, m_swapchain, kTimeout, m_semaphore_present, nullptr, &swap_chain_index));

    // // https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkFence.html
    // // A fence can be signaled as part of the execution of a queue submission command.
    // // Fences can be unsignaled on the host with vkResetFences.

    vkResetCommandBuffer(m_main_command_buffer, 0);

    VkCommandBufferBeginInfo cmd_begin_info = {};
    cmd_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    // cmd_begin_info.pNext = nullptr;

    // cmd_begin_info.pInheritanceInfo = nullptr;
    // cmd_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(m_main_command_buffer, &cmd_begin_info) != VK_SUCCESS)
    {
        std::cout << "oider" << std::endl;
    }

    VkClearValue clearValue;
    float flash = abs(sin(frame_number / 120.f));
    clearValue.color = {{0.0f, 0.0f, flash, 1.0f}};

    // // //start the main renderpass.
    // // //We will use the clear color from above, and the framebuffer of the index the swapchain gave us
    VkRenderPassBeginInfo rp_info = {};
    rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_info.pNext = nullptr;

    rp_info.renderPass = m_render_pass;
    rp_info.renderArea.offset.x = 0;
    rp_info.renderArea.offset.y = 0;
    rp_info.renderArea.extent = m_window_size;
    rp_info.framebuffer = m_frame_buffers[swap_chain_index];

    // // //connect clear values
    rp_info.clearValueCount = 1;
    rp_info.pClearValues = &clearValue;

    vkCmdBeginRenderPass(m_main_command_buffer, &rp_info, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdEndRenderPass(m_main_command_buffer);
    // // //finalize the command buffer (we can no longer add commands, but it can now be executed)
    VK_CHECK_RESULT(vkEndCommandBuffer(m_main_command_buffer));

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pNext = nullptr;

    // VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    // submit_info.pWaitDstStageMask = &wait_stage;

    VkSemaphore wait_semaphores[] = {m_semaphore_present};
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;

    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &m_semaphore_render;

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &m_main_command_buffer;

    vkResetFences(m_device, 1, &m_fence_render);

    // //submit command buffer to the queue and execute it.
    // // _renderFence will now block until the graphic commands finish execution
    vkQueueSubmit(m_graphics_queue, 1, &submit_info, m_fence_render);

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = NULL;
    VkSwapchainKHR swapChains[] = {m_swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &swap_chain_index;

    presentInfo.pWaitSemaphores = &m_semaphore_render;
    presentInfo.waitSemaphoreCount = 1;

    VkResult res = vkQueuePresentKHR(m_present_queue, &presentInfo);
    std::cout << "swap_chain_index:" << swap_chain_index << std::endl;
}

void Engine::run()
{
    size_t frame_number = 0;
    while (!glfwWindowShouldClose(m_window))
    {
        if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(m_window, true);
        }
        if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
            std::cout << "right mouse button pressed" << std::endl;
        }

        glfwPollEvents();
        draw(frame_number);
        frame_number++;
    }
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
