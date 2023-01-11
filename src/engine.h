#pragma once

#include <vulkan/vulkan.h>


class Engine {
    private:
        struct SDL_Window* m_window{ nullptr };  
    public: 
        void init(int width, int height);
        void cleanup();
        void draw();
        void run();
};