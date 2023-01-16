#include "entity.h"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include <vector>

namespace engine
{
struct ColoredVertex
{
    glm::vec3 position;
    glm::vec3 color;
};

class VisualComponent : public Component
{
  public:
    DEFINE_COMPONENT_ID(VisualComponent);
    VisualComponent()
    {
        m_vertices.resize(3);
        m_vertices[0].position = {1.f, 1.f, 0.0f};
        m_vertices[1].position = {-1.f, 1.f, 0.0f};
        m_vertices[2].position = {0.f, -1.f, 0.0f};

        // vertex colors, all green
        m_vertices[0].color = {1.f, 0.f, 0.0f};
        m_vertices[1].color = {0.f, 1.f, 0.0f};
        m_vertices[2].color = {0.f, 0.f, 1.0f};
    }
    std::vector<ColoredVertex>& vertices()
    {
        return m_vertices;
    };

  private:
    std::vector<ColoredVertex> m_vertices;
};

} // namespace engine