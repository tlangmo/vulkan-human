#pragma once
#include "vulkan/vulkan_core.h"
#include <vector>
namespace engine
{

/**
 * Abstract Base class for Vulkan Swapchain management.
 */
class SwapChain
{
  public:
    // disable copying of this object
    SwapChain() = default;
    SwapChain(const SwapChain&) = delete;
    SwapChain& operator=(const SwapChain& rhs) = delete;

    virtual ~SwapChain()
    {
        vkDestroySwapchainKHR(m_device, m_swapchain_khr, nullptr);
        for (auto sc : m_swapchain_image_views)
        {
            vkDestroyImageView(m_device, sc, nullptr);
        }
    }

    VkSwapchainKHR get_swapchain_khr()
    {
        return m_swapchain_khr;
    }
    VkFormat get_swapchain_format()
    {
        return m_swapchain_format;
    }
    const std::vector<VkImage>& get_swapchain_images() const
    {
        return m_swapchain_images;
    }
    const std::vector<VkImageView>& get_swapchain_image_views() const
    {
        return m_swapchain_image_views;
    }

    VkDevice m_device;
    VkSwapchainKHR m_swapchain_khr;
    VkFormat m_swapchain_format;
    std::vector<VkImage> m_swapchain_images;
    std::vector<VkImageView> m_swapchain_image_views;
};

class SimpleSwapchain : public SwapChain
{
  public:
    SimpleSwapchain(VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface);
};

} // namespace engine
