//
// Created by Sebastian Borsch on 13.07.23.
//

#if __APPLE__
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif

#include "../src/archetype.hpp"

class ComponentA{

    public:
        ComponentA() = default;
        ~ComponentA() = default;
};

TEST_CASE("Archetype - Create an empty Archetype and add new components.") {

    auto entity = 2;
    auto archetype01 = Archetype::createEmpty();
    archetype01->entities.push_back(entity);

    REQUIRE(archetype01->entities.size() == 1);
    REQUIRE(archetype01->entities[0] == entity);

    auto archetype02 = Archetype::createFromAdd<ComponentA>(archetype01);
    REQUIRE(archetype02.has_value());
    REQUIRE(archetype02.value()->entities.empty());
//    REQUIRE(archetype02.value()->containsType<ComponentA>());

//    archetype02.value()->migrateEntity(*archetype01, entity);
//    REQUIRE(archetype02.value()->entities.size() == 1);
//    REQUIRE(archetype02.value()->entities[0] == entity);
//    REQUIRE(archetype01->entities.empty());
}

