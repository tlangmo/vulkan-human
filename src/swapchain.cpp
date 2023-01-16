
#include "swapchain.h"
#include "VkBootstrap.h"

namespace engine
{

SimpleSwapchain::SimpleSwapchain(VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface)

{
    vkb::SwapchainBuilder vkb_swapchain_builder(physical_device, device, surface, 0, 0);
    vkb::Swapchain vkb_swapchain = vkb_swapchain_builder
                                       .use_default_format_selection()
                                       // use vsync present mode
                                       .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                                       .build()
                                       .value();
    // https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Physical_devices_and_queue_families
    m_swapchain_khr = vkb_swapchain.swapchain;
    m_swapchain_images = vkb_swapchain.get_images().value();
    m_swapchain_image_views = vkb_swapchain.get_image_views().value();
    m_swapchain_format = vkb_swapchain.image_format;
    m_device = device;
}

} // namespace engine
