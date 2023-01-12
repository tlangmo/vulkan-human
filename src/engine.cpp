
#include "engine.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <assert.h>
#include <iostream>

#include "VkBootstrap.h"

Engine::Engine() : m_swapchain(nullptr)
{
}

bool isDeviceSuitable(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
}

void Engine::init(int width, int height)
{
    // We initialize SDL and create a window with it.

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(width, height, "Vulkan window", nullptr, nullptr);
    
    uint32_t ext_count = 0;
    const char** glfw_extensions;

    glfw_extensions = glfwGetRequiredInstanceExtensions(&ext_count);


    vkb::InstanceBuilder builder;
    builder.set_app_name("Vulkan Human")
        .request_validation_layers(true)
        .use_default_debug_messenger()
        .require_api_version(1, 1, 0);
    for (int i=0; i < ext_count; i++) {
        builder.enable_extension(glfw_extensions[i]);
    }

    vkb::Instance vkb_instance = builder.build().value();
    m_instance = vkb_instance.instance;
    m_debug_messenger = vkb_instance.debug_messenger;

   if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }

    // use vkbootstrap to select a GPU.
    vkb::PhysicalDeviceSelector selector{vkb_instance};
    vkb::PhysicalDevice vkb_physical_device = selector.set_minimum_version(1,  2).set_surface(m_surface).prefer_gpu_device_type().require_present(true).select().value();

    vkb::DeviceBuilder vkb_device_builder{vkb_physical_device};
    vkb::Device vkb_device = vkb_device_builder.build().value();

    // Get the VkDevice handle used in the rest of a Vulkan application
    m_device = vkb_device.device;
    m_physical_device = vkb_physical_device.physical_device;

    init_swapchain();
}

void Engine::init_swapchain()
{
    vkb::SwapchainBuilder vkb_swapchain_builder(m_physical_device, m_device, m_surface);
    int w = 0;
    int h = 0;
    glfwGetWindowSize(m_window, &w, &h);

    vkb::Swapchain vkb_swapchain = vkb_swapchain_builder
                                       .use_default_format_selection()
                                       // use vsync present mode
                                       .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                                       .set_desired_extent(w, h)
                                       .build()
                                       .value();
    // https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Physical_devices_and_queue_families
    m_swapchain = vkb_swapchain.swapchain;
    m_swapchain_images = vkb_swapchain.get_images().value();
    m_swapchain_image_views = vkb_swapchain.get_image_views().value();
    m_swapchain_format = vkb_swapchain.image_format;
}

void Engine::cleanup()
{
    if (m_swapchain)
    {
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
        for (auto sc : m_swapchain_image_views)
        {
            vkDestroyImageView(m_device, sc, nullptr);
        }
    }
    vkDestroyDevice(m_device, nullptr);
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);
    vkDestroyInstance(m_instance, nullptr);

}
void Engine::draw()
{
}

void Engine::run()
{
    while(!glfwWindowShouldClose(m_window)) {
        if(glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(m_window, true);
        }
        if( glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
             std::cout << "right mouse button pressed" << std::endl;
        }

        glfwPollEvents();
        draw();
    }
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
