#include "entity.h"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

namespace engine
{
class CoordSysComponent : public Component
{
  public:
    DEFINE_COMPONENT_ID(CoordSysComponent);
    CoordSysComponent(){};
    void set_translation(const glm::vec3& trans)
    {
        m_trans = trans;
    }
    const glm::vec3& translation()
    {
        return m_trans;
    }

  private:
    glm::mat4x4 m_mat_world;
    glm::vec3 m_trans;
};
} // namespace engine