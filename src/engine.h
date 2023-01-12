#pragma once

#include <vector>
#include <vulkan/vulkan.h>
class Engine
{
  private:
    struct GLFWwindow* m_window{nullptr};

    VkInstance m_instance;                      // Vulkan library handle
    VkDebugUtilsMessengerEXT m_debug_messenger; // Vulkan debug output handle
    VkPhysicalDevice m_physical_device;
    VkDevice m_device;      // Vulkan device for commands
    VkSurfaceKHR m_surface; // Vulkan window surface

    VkSwapchainKHR m_swapchain;
    VkFormat m_swapchain_format;
    std::vector<VkImage> m_swapchain_images;
    std::vector<VkImageView> m_swapchain_image_views;

  private:
    void init_swapchain();

  public:
    Engine();
    void init(int width, int height);
    void cleanup();
    void draw();
    void run();
};