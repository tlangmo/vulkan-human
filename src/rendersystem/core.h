#pragma once
#include "VkBootstrap.h"
#include <string>
#include <vulkan/vulkan_core.h>
// forward decl
VK_DEFINE_HANDLE(VmaAllocator)

struct GLFWwindow;

namespace rendersystem
{

/**
 * Hold data for Vulkan core objects.
 */
struct CoreData
{
    GLFWwindow* window{nullptr};
    VkExtent2D window_size;
    VkInstance instance;                      // Vulkan library handle
    VkDebugUtilsMessengerEXT debug_messenger; // Vulkan debug output handle
    VkPhysicalDevice physical_device;
    VkDevice device;      // Vulkan device for commands
    VkSurfaceKHR surface; // Vulkan window surface
    VkQueue graphics_queue;
    uint32_t graphics_queue_family;
    VkQueue present_queue;
    uint32_t present_queue_family;
    VmaAllocator allocator;
    VkCommandPool cmd_pool;
    VkCommandBuffer cmd_buf_main;
    VkSemaphore semaphore_present;
    VkSemaphore semaphore_render;
    VkFence fence_host;
};
/**
 * Create Vulkan objects for on-screen rendering
 */
CoreData create_core_with_window(const std::string& app_name, uint32_t width, uint32_t height);

void destroy_core(CoreData* core_data);

} // namespace rendersystem