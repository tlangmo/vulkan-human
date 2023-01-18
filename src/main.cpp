#include <stdio.h>

#include "components/coordsys.h"
#include "components/visual.h"
#include "entity.h"
#include "glm/gtx/transform.hpp"
#include "rendersystem.h"
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
    e.add_component(VisualComponent::from_gltf_file("../assets/torus.gltf"));
    return e;
}
int main(int argc, char* argv[])
{
    auto e0 = create_triangle();
    auto e1 = create_torus();
    e0.get_component<CoordSysComponent>()->mat_world() = glm::translate(glm::vec3(1, 0, 0));
    std::vector<Entity> entities = {e0, e1};
    RenderSystem rs;
    rs.create(800, 600);
    rs.process(entities);
    rs.destroy();
    return 0;
}
