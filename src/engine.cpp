#include "engine.h"

#include <SDL.h>
#include <SDL_vulkan.h>
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
    SDL_Init(SDL_INIT_VIDEO);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

    // create blank SDL window for our application
    m_window = SDL_CreateWindow("Vulkan Engine",         // window title
                                SDL_WINDOWPOS_UNDEFINED, // window position x (don't care)
                                SDL_WINDOWPOS_UNDEFINED, // window position y (don't care)
                                width,                   // window width in pixels
                                height,                  // window height in pixels
                                window_flags);

    unsigned int ext_count = 100;
    const char* ext_names[100];
    SDL_bool re = SDL_Vulkan_GetInstanceExtensions(m_window, &ext_count, ext_names);

    vkb::InstanceBuilder builder;
    builder.set_app_name("Vulkan Human")
        .request_validation_layers(true)
        .use_default_debug_messenger()
        .require_api_version(1, 1, 0);

    for (int i = 0; i < ext_count; i++)
    {
        builder.enable_extension(ext_names[i]);
    }

    vkb::Instance vkb_instance = builder.build().value();
    m_instance = vkb_instance.instance;
    m_debug_messenger = vkb_instance.debug_messenger;

    SDL_bool res = SDL_Vulkan_CreateSurface(m_window, m_instance, &m_surface);
    // get the surface of the window we opened with SDL
    assert(res == SDL_TRUE);

    // use vkbootstrap to select a GPU.
    // We want a GPU that can write to the SDL surface and supports Vulkan 1.1
    vkb::PhysicalDeviceSelector selector{vkb_instance};
    vkb::PhysicalDevice vkb_physical_device = selector.set_surface(m_surface).select().value();
    //   selector.set_minimum_version(1,
    //   2).set_surface(m_surface).prefer_gpu_device_type().require_present(true).select().value();

    vkb::DeviceBuilder vkb_device_builder{vkb_physical_device};
    vkb::Device vkb_device = vkb_device_builder.build().value();

    // Get the VkDevice handle used in the rest of a Vulkan application
    m_device = vkb_device.device;
    m_physical_device = vkb_physical_device.physical_device;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        if (isDeviceSuitable(device))
        {
            m_physical_device = device;
            // break;
        }
    }

    if (m_physical_device == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    init_swapchain();
}

void Engine::init_swapchain()
{
    vkb::SwapchainBuilder vkb_swapchain_builder(m_physical_device, m_device, m_surface);
    int w = 0;
    int h = 0;
    SDL_GetWindowSize(m_window, &w, &h);

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

    SDL_DestroyWindow(m_window);
}
void Engine::draw()
{
}
void Engine::run()
{
    SDL_Event e;
    bool should_run = true;

    // main loop
    while (should_run)
    {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0)
        {
            switch (e.type)
            {
            case SDL_QUIT: {
                should_run = false;
            }
            break;
            case SDL_MOUSEBUTTONDOWN: {
                std::cout << "mouse button pressed" << std::endl;
            }
            break;
            default:;
            };
        }

        draw();
    }
}
