
#include "swapchain.h"
#include "VkBootstrap.h"
#include "check.h"
#include "vk_mem_alloc.h"

namespace rendersystem
{

static void create_depth_image(const CoreData& core_data, VkImage* depth_img, VkImageView* depth_img_view,
                               VmaAllocation* alloc, VkFormat format)
{
    VkExtent3D depth_image_extent = {core_data.window_size.width, core_data.window_size.height, 1};

    VkImageCreateInfo dimg_info = {};
    dimg_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    dimg_info.pNext = nullptr;

    dimg_info.imageType = VK_IMAGE_TYPE_2D;

    dimg_info.format = format;
    dimg_info.extent = depth_image_extent;

    dimg_info.mipLevels = 1;
    dimg_info.arrayLayers = 1;
    dimg_info.samples = VK_SAMPLE_COUNT_1_BIT;
    dimg_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    dimg_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    VmaAllocationCreateInfo dimg_allocinfo = {};
    dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    dimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // allocate and create the image
    vmaCreateImage(core_data.allocator, &dimg_info, &dimg_allocinfo, depth_img, alloc, nullptr);

    // build a image-view for the depth image to use for rendering
    {
        VkImageViewCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.pNext = nullptr;

        info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        info.image = *depth_img;
        info.format = dimg_info.format;
        info.subresourceRange.baseMipLevel = 0;
        info.subresourceRange.levelCount = 1;
        info.subresourceRange.baseArrayLayer = 0;
        info.subresourceRange.layerCount = 1;
        info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        VK_CHECK_RESULT(vkCreateImageView(core_data.device, &info, nullptr, depth_img_view));
    }
}

SwapChainData create_swapchain(const CoreData& core_data)
{
    SwapChainData sd = {};
    vkb::SwapchainBuilder vkb_swapchain_builder(core_data.physical_device, core_data.device, core_data.surface, 0, 0);
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

    create_depth_image(core_data, &sd.depth_image, &sd.depth_image_view, &sd.depth_image_allocation,
                       VK_FORMAT_D32_SFLOAT);
    sd.depth_image_format = VK_FORMAT_D32_SFLOAT;
    return sd;
}
void destroy_swapchain(const CoreData& core_data, SwapChainData* sd)
{
    vkDestroySwapchainKHR(core_data.device, sd->swapchain_khr, nullptr);
    for (auto sc : sd->swapchain_image_views)
    {
        vkDestroyImageView(core_data.device, sc, nullptr);
    }
    vkDestroyImageView(core_data.device, sd->depth_image_view, nullptr);
    vmaDestroyImage(core_data.allocator, sd->depth_image, sd->depth_image_allocation);
    *sd = {};
}

} // namespace rendersystem
