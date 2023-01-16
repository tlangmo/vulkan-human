#include <stdio.h>

#include "components/coordsys.h"
#include "components/visual.h"
#include "entity.h"
#include "rendersystem.h"
#include <iostream>
using namespace engine;

Entity create_cube()
{
    Entity e;
    auto coords = std::make_shared<CoordSysComponent>();
    coords->set_translation(glm::vec3(0, 1, 2));
    e.add_component(coords);
    e.add_component(std::make_shared<VisualComponent>());
    return e;
}
int main(int argc, char* argv[])
{
    auto e0 = create_cube();
    auto e1 = create_cube();
    e1.get_component<VisualComponent>()->vertices()[0].position.y = 0.5;

    std::vector<Entity> entities = {e0, e1};
    RenderSystem rs;
    rs.create(800, 600);
    rs.process(entities);
    rs.destroy();
    return 0;
}
