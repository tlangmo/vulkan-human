#pragma once
#include "vulkan/vulkan_core.h"
#include <functional>
#include <vector>
namespace engine
{

/**
 * Abstract Base class for Vulkan Swapchain management.
 */
struct SwapChainData
{
    VkSwapchainKHR swapchain_khr;
    VkFormat swapchain_format;
    std::vector<VkImage> swapchain_images;
    std::vector<VkImageView> swapchain_image_views;
    std::vector<std::function<void()>> deletors;
};

SwapChainData create_swapchain(VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface);

} // namespace engine
