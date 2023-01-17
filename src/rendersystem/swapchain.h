#pragma once
#include "vulkan/vulkan_core.h"
#include <functional>
#include <vector>
namespace rendersystem
{

/**
 * Hold data for Vulkan swapchain management.
 */
struct SwapChainData
{
    VkSwapchainKHR swapchain_khr;
    VkFormat swapchain_format;
    std::vector<VkImage> swapchain_images;
    std::vector<VkImageView> swapchain_image_views;
};

SwapChainData create_swapchain(VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface);
void destroy_swapchain(VkDevice device, SwapChainData* sd);
} // namespace rendersystem
