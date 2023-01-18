#pragma once
#include "core.h"
#include "vulkan/vulkan_core.h"
#include <functional>
#include <vector>

// forward decl
VK_DEFINE_HANDLE(VmaAllocation)

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

    VkImage depth_image;
    VkFormat depth_image_format;
    VkImageView depth_image_view;
    VmaAllocation depth_image_allocation;
};

SwapChainData create_swapchain(const CoreData& core_data);
void destroy_swapchain(const CoreData& core_data, SwapChainData* sd);
} // namespace rendersystem
