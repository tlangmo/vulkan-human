#pragma once

#include "VkBootstrap.h"
#include "mesh.h"
#include <vector>
#include <vulkan/vulkan.h>

// forward decl
VK_DEFINE_HANDLE(VmaAllocator)

class Engine
{
  private:
    struct GLFWwindow* m_window{nullptr};
    VkExtent2D m_window_size;
    VkInstance m_instance;                      // Vulkan library handle
    VkDebugUtilsMessengerEXT m_debug_messenger; // Vulkan debug output handle
    VkPhysicalDevice m_physical_device;
    VkDevice m_device;      // Vulkan device for commands
    VkSurfaceKHR m_surface; // Vulkan window surface

    VkSwapchainKHR m_swapchain;
    VkFormat m_swapchain_format;
    std::vector<VkImage> m_swapchain_images;
    std::vector<VkImageView> m_swapchain_image_views;

    std::vector<VkFramebuffer> m_frame_buffers;

    VkQueue m_graphics_queue;
    uint32_t m_graphics_queue_family;
    VkQueue m_present_queue;
    uint32_t m_present_queue_family;
    VkCommandPool m_command_pool;
    VkCommandBuffer m_main_command_buffer;
    VkRenderPass m_render_pass;

    VkSemaphore m_semaphore_present;
    VkSemaphore m_semaphore_render;
    VkFence m_fence_render;

    VkPipeline m_pipeline;
    VkPipelineLayout m_pipeline_layout;
    VkShaderModule m_triangle_frag;
    VkShaderModule m_triangle_vert;

    VmaAllocator m_allocator;
    Mesh m_mesh;

  private:
    void init_swapchain(vkb::Device dev);
    void init_commands();
    void init_default_renderpass();
    void init_framebuffers();
    void init_barriers();
    void load_shader_module(const char* file_path, VkShaderModule* out_shader_module);
    void init_pipelines();
    void init_scene_data();

  public:
    Engine();
    void init(uint32_t width, uint32_t height);
    void cleanup();
    void draw(size_t frame_number);
    void run();
};