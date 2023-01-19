#pragma once
#include "entity.h"
#include "glm/vec2.hpp"
#include <GLFW/glfw3.h>
#include <unordered_map>
namespace inputsystem
{

class InputSystem
{
  public:
    InputSystem(GLFWwindow* app_window);
    void on_key(int key, int scancode, int action, int mods);
    void on_mouse(glm::vec2 mouse_pos);
    void process(const std::vector<components::Entity>& entities, uint64_t elapsed_us);

  private:
    GLFWwindow* m_app_window;
    std::unordered_map<int, bool> m_active_keys;
    glm::vec2 m_prev_mouse_pos;
    glm::vec2 m_cur_mouse_pos;
    int m_width;
    int m_height;
};

} // namespace inputsystem