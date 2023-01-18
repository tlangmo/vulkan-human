#include <stdio.h>

#include "components/camera.h"
#include "components/coordsys.h"
#include "components/visual.h"
#include "entity.h"
#include "glm/gtx/transform.hpp"
#include "rendersystem.h"
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>
using namespace rendersystem;

Entity create_triangle()
{
    Entity e;
    auto coords = std::make_shared<CoordSysComponent>();
    e.add_component(coords);
    e.add_component(VisualComponent::make_triangle());
    return e;
}

Entity create_torus()
{
    Entity e;
    auto coords = std::make_shared<CoordSysComponent>();
    e.add_component(coords);
    e.add_component(VisualComponent::from_gltf_file("../assets/torus_smooth.gltf"));
    return e;
}

Entity create_camera(float aspect)
{
    Entity e;
    auto cam = std::make_shared<CameraComponent>(aspect, 70.0f);
    cam->view_mat() = glm::translate(glm::vec3(0, 0, -2));
    e.add_component(cam);
    return e;
}
int main(int argc, char* argv[])
{
    auto e0 = create_triangle();
    auto e1 = create_torus();
    auto cam = create_camera(4 / 3.0f);
    e0.get_component<CoordSysComponent>()->mat_world() = glm::translate(glm::vec3(1, 0, 0));
    std::vector<Entity> entities = {e0, e1, cam};

    RenderSystem rs;
    GLFWwindow* app_window = rs.create(800, 600);
    auto prev_ts = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(app_window))
    {
        if (glfwGetKey(app_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(app_window, true);
        }
        if (glfwGetMouseButton(app_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
            std::cout << "right mouse button pressed" << std::endl;
        }
        glfwPollEvents();
        auto now_ts = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(now_ts - prev_ts);
        prev_ts = now_ts;
        rs.process(entities, elapsed_time.count());
    }
    glfwDestroyWindow(app_window);
    glfwTerminate();
    rs.destroy();
    return 0;
}
