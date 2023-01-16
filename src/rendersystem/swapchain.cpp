
#include "swapchain.h"
#include "VkBootstrap.h"

namespace engine
{
SwapChainData create_swapchain(VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface)
{

    SwapChainData sd = {};
    vkb::SwapchainBuilder vkb_swapchain_builder(physical_device, device, surface, 0, 0);
    vkb::Swapchain vkb_swapchain = vkb_swapchain_builder
                                       .use_default_format_selection()
                                       // use vsync present mode
                                       .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                                       .build()
                                       .value();
    // https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Physical_devices_and_queue_families
    sd.swapchain_khr = vkb_swapchain.swapchain;
    sd.swapchain_images = vkb_swapchain.get_images().value();
    sd.swapchain_image_views = vkb_swapchain.get_image_views().value();
    sd.swapchain_format = vkb_swapchain.image_format;

    sd.deletors.push_back([device, sd]() { vkDestroySwapchainKHR(device, sd.swapchain_khr, nullptr); });
    sd.deletors.push_back([device, sd]() {
        for (auto sc : sd.swapchain_image_views)
        {
            vkDestroyImageView(device, sc, nullptr);
        }
    });

    return sd;
}

} // namespace engine
