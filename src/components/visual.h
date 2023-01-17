#include "entity.h"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "tiny_gltf.h"
#include <string>
#include <vector>

namespace rendersystem
{

struct StandardVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 color;
};

class VisualComponent : public Component
{
  public:
    DEFINE_COMPONENT_ID(VisualComponent);
    VisualComponent()
    {
    }
    std::vector<StandardVertex>& vertices()
    {
        return m_vertices;
    };
    std::vector<uint32_t>& indicies()
    {
        return m_indicies;
    };
    static std::shared_ptr<VisualComponent> test_triangle();
    static std::shared_ptr<VisualComponent> from_gltf_file(const std::string& fn);

  private:
    std::vector<StandardVertex> m_vertices;
    std::vector<uint32_t> m_indicies;
};

std::vector<StandardVertex> create_triangle_data();

} // namespace rendersystem