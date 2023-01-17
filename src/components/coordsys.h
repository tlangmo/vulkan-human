#include "entity.h"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

namespace rendersystem
{
class CoordSysComponent : public Component
{
  public:
    DEFINE_COMPONENT_ID(CoordSysComponent);
    CoordSysComponent() : m_mat_world{1.0}, m_trans{} {};
    const glm::vec3& translation() const
    {
        return m_trans;
    }
    glm::vec3& translation()
    {
        return m_trans;
    }
    glm::mat4x4& mat_world()
    {
        return m_mat_world;
    }
    const glm::mat4x4& mat_world() const
    {
        return m_mat_world;
    }

  private:
    glm::mat4x4 m_mat_world;
    glm::vec3 m_trans;
};
} // namespace rendersystem