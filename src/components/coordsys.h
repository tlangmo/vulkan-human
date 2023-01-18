#include "entity.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

namespace rendersystem
{
class CoordSysComponent : public Component
{
  public:
    DEFINE_COMPONENT_ID(CoordSysComponent);
    CoordSysComponent() : m_translation{}, m_rotation{} {};
    const glm::vec3& translation() const
    {
        return m_translation;
    }
    glm::vec3& rotation()
    {
        return m_rotation;
    }
    const glm::vec3& rotation() const
    {
        return m_rotation;
    }
    glm::vec3& translation()
    {
        return m_translation;
    }

    glm::mat4x4 mat_world() const
    {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), m_translation);
        m = glm::translate(glm::mat4(1.0f), m_translation) *
            glm::rotate(glm::mat4(1.0f), m_rotation[0], glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), m_rotation[1], glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), m_rotation[2], glm::vec3(0.0f, 0.0f, 1.0f));

        return m;
    }

  private:
    glm::vec3 m_rotation;
    glm::vec3 m_translation;
};
} // namespace rendersystem