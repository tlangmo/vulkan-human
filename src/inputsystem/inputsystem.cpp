#include "inputsystem.h"
#include "camera.h"
#include "coordsys.h"
namespace inputsystem
{

InputSystem* instance = nullptr;

static void static_on_key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    InputSystem& insystem = *instance;
    insystem.on_key(key, scancode, action, mods);
}

static void static_on_mouse(GLFWwindow* window, double x, double y)
{
    InputSystem& insystem = *instance;
    insystem.on_mouse(glm::vec2(x, y));
}

// https://stackoverflow.com/questions/46631814/handling-multiple-keys-input-at-once-with-glfw

void InputSystem::on_key(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        m_active_keys[key] = true;
    }
    if (action == GLFW_RELEASE)
    {
        m_active_keys.erase(key);
    }
}

void InputSystem::on_mouse(glm::vec2 mouse_pos)
{
    std::cout << "mouse x: " << mouse_pos.x << ", y: " << mouse_pos.y << std::endl;
}

void InputSystem::process(const std::vector<components::Entity>& entities, uint64_t elapsed_us)
{
    float elapsed_sec = (float)elapsed_us / 1'000'000;
    for (auto& e : entities)
    {
        if (auto cam = e.get_component<components::Camera>(); cam != nullptr)
        {
            float speed = cam->sensitivity();
            if (m_active_keys.find(GLFW_KEY_W) != m_active_keys.end())
            {
                cam->coordsys().position() += speed * (float)elapsed_sec * glm::vec3{0, 0, 1.0f};
            }
            if (m_active_keys.find(GLFW_KEY_S) != m_active_keys.end())
            {
                cam->coordsys().position() += speed * (float)elapsed_sec * glm::vec3{0, 0, -1.0f};
            }
            if (m_active_keys.find(GLFW_KEY_A) != m_active_keys.end())
            {
                cam->coordsys().position() += speed * (float)elapsed_sec * glm::vec3{-1, 0, 0.0f};
            }
            if (m_active_keys.find(GLFW_KEY_D) != m_active_keys.end())
            {
                cam->coordsys().position() += speed * (float)elapsed_sec * glm::vec3{1, 0, 0.0f};
            }
            if (m_active_keys.find(GLFW_KEY_R) != m_active_keys.end())
            {
                cam->reset();
            }
        }
    }
}

InputSystem::InputSystem(GLFWwindow* app_window) : m_app_window(app_window)
{
    instance = this;
    glfwSetKeyCallback(app_window, static_on_key);
    glfwSetCursorPosCallback(app_window, static_on_mouse);
    glfwSetInputMode(app_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

} // namespace inputsystem