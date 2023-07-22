//
// Created by Sebastian Borsch on 01.07.23.
//
#if __APPLE__
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif

#include "../src/entity-generator.hpp"

TEST_CASE("Add new entity - Entity is 0") {
    auto *eg = new EntityGenerator();
    auto newEntity = eg->createEntity();
    REQUIRE(newEntity.has_value());
    REQUIRE(newEntity.value() == 0);
}

TEST_CASE("Remove an existing entity - Entity should be labeled as dead"){
    auto *eg = new EntityGenerator();
    auto entity_opt = eg->createEntity();
    REQUIRE(entity_opt.has_value());
    auto entity = entity_opt.value();
    eg->removeEntity(entity);
    REQUIRE(eg->isAlive(entity) == false);
}

TEST_CASE("Remove an invalid entity - Expected Exception"){
    auto *eg = new EntityGenerator();
    Entity entity = 15;

    REQUIRE_THROWS(eg->removeEntity(entity));
}

TEST_CASE("Remove already dead entity - Nothing happens"){
    auto *eg = new EntityGenerator();
    auto entity_opt = eg->createEntity();
    REQUIRE(entity_opt.has_value());
    auto entity = entity_opt.value();
    eg->removeEntity(entity);
    REQUIRE_NOTHROW(eg->removeEntity(entity));

}
