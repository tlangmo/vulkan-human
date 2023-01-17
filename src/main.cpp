#include <stdio.h>

#include "components/coordsys.h"
#include "components/visual.h"
#include "entity.h"
#include "glm/gtx/transform.hpp"
#include "rendersystem.h"
#include <iostream>
using namespace rendersystem;

Entity create_cube()
{
    Entity e;
    auto coords = std::make_shared<CoordSysComponent>();
    e.add_component(coords);
    e.add_component(std::make_shared<VisualComponent>());
    return e;
}
int main(int argc, char* argv[])
{
    auto e0 = create_cube();
    auto e1 = create_cube();
    e1.get_component<VisualComponent>()->vertices()[0].position.y = 0.5;
    e1.get_component<CoordSysComponent>()->mat_world() = glm::translate(glm::vec3(1, 0, 0));
    e0.get_component<CoordSysComponent>()->mat_world() = glm::translate(glm::vec3(-1, 0, 0));
    std::vector<Entity> entities = {e0, e1};
    RenderSystem rs;
    rs.create(800, 600);
    rs.process(entities);
    rs.destroy();
    return 0;
}
