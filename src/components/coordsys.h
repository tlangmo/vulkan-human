#pragma once
#include "entity.h"

#include "glm/gtc/quaternion.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

namespace rendersystem
{
class CoordSysComponent : public Component
{
  public:
    DEFINE_COMPONENT_ID(CoordSysComponent);
    CoordSysComponent() : m_rotation{1, 0, 0, 0}, m_position{} {};

    const glm::vec3& position() const
    {
        return m_position;
    }
    glm::vec3& position()
    {
        return m_position;
    }

    glm::quat& rotation()
    {
        return m_rotation;
    }
    const glm::quat& rotation() const
    {
        return m_rotation;
    }

    glm::mat4x4 transform() const
    {
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), m_position);
        glm::mat4 rot = glm::mat4_cast(m_rotation);
        return trans * rot;
    }

  private:
    glm::vec3 m_position;
    glm::quat m_rotation;
};
} // namespace rendersystem