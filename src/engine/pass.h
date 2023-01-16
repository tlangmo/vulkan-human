#pragma once
#include "core.h"
#include "swapchain.h"
#include <functional>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace engine
{

struct PassData
{
    std::vector<VkFramebuffer> frame_buffers;
    VkRenderPass render_pass;
    std::vector<std::function<void()>> deletors;
};

PassData create_simple_pass(const CoreData& core_data, const SwapChainData& swap_chain_data);

} // namespace engine