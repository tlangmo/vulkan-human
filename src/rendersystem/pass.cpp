
#include "pass.h"
#include "check.h"
#include "core.h"
#include "swapchain.h"
#include <algorithm>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace rendersystem
{

PassData create_basic_pass(const CoreData& core_data, const SwapChainData& swap_chain_data)
{
    // a nice explanation: https://developer.samsung.com/galaxy-gamedev/resources/articles/renderpasses.html
    PassData rd;
    VkAttachmentDescription color_attachment = {};
    color_attachment.format = swap_chain_data.swapchain_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = {};
    // attachment number will index into the pAttachments array in the parent renderpass itself
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.pNext = nullptr;
    // connect the color attachment to the info
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    // connect the subpass to the info
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;

    vkCreateRenderPass(core_data.device, &render_pass_info, nullptr, &rd.render_pass);

    VkFramebufferCreateInfo fb_info = {};
    fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_info.pNext = nullptr;

    fb_info.renderPass = rd.render_pass;
    fb_info.attachmentCount = 1;
    fb_info.width = core_data.window_size.width;
    fb_info.height = core_data.window_size.height;
    fb_info.layers = 1;

    rd.frame_buffers.clear();

    std::transform(swap_chain_data.swapchain_image_views.begin(), swap_chain_data.swapchain_image_views.end(),
                   std::back_inserter(rd.frame_buffers), [&](const VkImageView img) -> VkFramebuffer {
                       fb_info.pAttachments = &img;
                       VkFramebuffer fb;
                       VK_CHECK_RESULT(vkCreateFramebuffer(core_data.device, &fb_info, nullptr, &fb));
                       return fb;
                   });
    return rd;
}

void destroy_pass(VkDevice device, PassData* rd)
{
    vkDestroyRenderPass(device, rd->render_pass, nullptr);
    for (auto b : rd->frame_buffers)
    {
        vkDestroyFramebuffer(device, b, nullptr);
    }
    rd = {};
}

} // namespace rendersystem