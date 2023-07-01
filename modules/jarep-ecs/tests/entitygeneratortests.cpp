//
// Created by Sebastian Borsch on 01.07.23.
//

#include <catch2/catch_test_macros.hpp>
#include "../src/entitygenerator.hpp"

TEST_CASE("Add new entity"){
    auto *eg = new EntityGenerator();
    Entity newEntity = eg->createEntity();
    REQUIRE(newEntity == 1);
}
