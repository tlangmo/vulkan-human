#include "core.h"
#define GLFW_INCLUDE_VULKAN
#include "VkBootstrap.h"
#include <GLFW/glfw3.h>

#define VMA_IMPLEMENTATION
#include "check.h"
#include "vk_mem_alloc.h"
namespace rendersystem
{

CoreData create_core_with_window(const std::string& app_name, uint32_t width, uint32_t height)
{

    CoreData core_data = {};
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    core_data.window = glfwCreateWindow(width, height, app_name.c_str(), nullptr, nullptr);
    core_data.window_size = VkExtent2D{width, height};
    uint32_t ext_count = 0;
    const char** glfw_extensions;

    glfw_extensions = glfwGetRequiredInstanceExtensions(&ext_count);

    vkb::InstanceBuilder builder;
    builder.set_app_name(app_name.c_str()).request_validation_layers(true).use_default_debug_messenger();
    for (int i = 0; i < ext_count; i++)
    {
        builder.enable_extension(glfw_extensions[i]);
    }

    vkb::Instance vkb_instance = builder.build().value();
    core_data.instance = vkb_instance.instance;
    core_data.debug_messenger = vkb_instance.debug_messenger;

    if (glfwCreateWindowSurface(core_data.instance, core_data.window, nullptr, &core_data.surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }

    // use vkbootstrap to select a GPU.
    vkb::PhysicalDeviceSelector selector{vkb_instance};
    vkb::PhysicalDevice vkb_physical_device = selector
                                                  .set_surface(core_data.surface)
                                                  //   .prefer_gpu_device_type()
                                                  //   .require_present(true)
                                                  .select()
                                                  .value();

    vkb::DeviceBuilder vkb_device_builder{vkb_physical_device};
    vkb::Device vkb_device = vkb_device_builder.build().value();

    // Get the VkDevice handle used in the rest of a Vulkan application
    core_data.device = vkb_device.device;
    core_data.physical_device = vkb_physical_device.physical_device;
    core_data.graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
    core_data.graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

    core_data.present_queue = vkb_device.get_queue(vkb::QueueType::present).value();
    core_data.present_queue_family = vkb_device.get_queue_index(vkb::QueueType::present).value();

    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = core_data.physical_device;
    allocatorInfo.device = core_data.device;
    allocatorInfo.instance = core_data.instance;
    vmaCreateAllocator(&allocatorInfo, &core_data.allocator);

    VkCommandPoolCreateInfo cmd_pool_info = {};
    cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmd_pool_info.pNext = nullptr;
    cmd_pool_info.queueFamilyIndex = core_data.graphics_queue_family;
    cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    vkCreateCommandPool(core_data.device, &cmd_pool_info, nullptr, &core_data.cmd_pool);

    VkCommandBufferAllocateInfo cmd_buf_info = {};
    cmd_buf_info.pNext = nullptr;
    cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd_buf_info.commandPool = core_data.cmd_pool;
    cmd_buf_info.commandBufferCount = 1;
    cmd_buf_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    vkAllocateCommandBuffers(core_data.device, &cmd_buf_info, &core_data.cmd_buf_main);

    VkFenceCreateInfo fence_create_info = {};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    vkCreateFence(core_data.device, &fence_create_info, nullptr, &core_data.fence_host);

    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VK_CHECK_RESULT(vkCreateSemaphore(core_data.device, &semaphore_create_info, nullptr, &core_data.semaphore_present));
    VK_CHECK_RESULT(vkCreateSemaphore(core_data.device, &semaphore_create_info, nullptr, &core_data.semaphore_render));

    return core_data;
}

void destroy_core(CoreData* core_data)
{
    vkDestroyFence(core_data->device, core_data->fence_host, nullptr);
    vkDestroySemaphore(core_data->device, core_data->semaphore_present, nullptr);
    vkDestroySemaphore(core_data->device, core_data->semaphore_render, nullptr);
    vkDestroyCommandPool(core_data->device, core_data->cmd_pool, nullptr);
    vmaDestroyAllocator(core_data->allocator);
    vkDestroyDevice(core_data->device, nullptr);
    vkDestroySurfaceKHR(core_data->instance, core_data->surface, nullptr);
    vkb::destroy_debug_utils_messenger(core_data->instance, core_data->debug_messenger);
    vkDestroyInstance(core_data->instance, nullptr);
    *core_data = {};
}

} // namespace rendersystem
