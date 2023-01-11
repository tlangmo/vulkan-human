#include "engine.h"

#include <SDL.h>
#include <SDL_vulkan.h>

#include <iostream>

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
}
void Engine::cleanup()
{
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
