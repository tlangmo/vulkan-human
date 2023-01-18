
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

    VkAttachmentDescription depth_attachment = {};
    depth_attachment.format = swap_chain_data.depth_image_format;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_ref = {};
    // attachment number will index into the pAttachments array in the parent renderpass itself
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.pDepthStencilAttachment = &depth_attachment_ref;

    VkAttachmentDescription attachments[2] = {color_attachment, depth_attachment};

    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.pNext = nullptr;
    // connect the color attachment to the info
    render_pass_info.attachmentCount = 2;
    render_pass_info.pAttachments = attachments;
    // connect the subpass to the info
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkSubpassDependency depth_dependency = {};
    depth_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    depth_dependency.dstSubpass = 0;
    depth_dependency.srcStageMask =
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depth_dependency.srcAccessMask = 0;
    depth_dependency.dstStageMask =
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depth_dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkSubpassDependency dependencies[2] = {dependency, depth_dependency};
    render_pass_info.dependencyCount = 2;
    render_pass_info.pDependencies = dependencies;

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
                   std::back_inserter(rd.frame_buffers), [&](const VkImageView swap_chain_img_view) -> VkFramebuffer {
                       VkImageView attachments[2] = {swap_chain_img_view, swap_chain_data.depth_image_view};
                       fb_info.pAttachments = attachments;
                       fb_info.attachmentCount = 2;
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