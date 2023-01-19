#include "entity.h"
#include <catch2/catch_test_macros.hpp>
#include <cstring>

using namespace components;

class MyComponent : public Component
{
  public:
    DEFINE_COMPONENT_ID(MyComponent)
};

class OtherComponent : public Component
{
  public:
    DEFINE_COMPONENT_ID(OtherComponent)
};

TEST_CASE("Adding components to an entity")
{
    Entity e;
    e.add_component(std::make_shared<MyComponent>());

    auto e2 = e.get_component<MyComponent>();
    REQUIRE(e2->id() == COMPONENT_ID(MyComponent));

    auto e3 = e.get_component<OtherComponent>();
    REQUIRE(e3 == nullptr);
    e.add_component(std::make_shared<OtherComponent>());
    e3 = e.get_component<OtherComponent>();
    REQUIRE(e3->id() == COMPONENT_ID(OtherComponent));
}