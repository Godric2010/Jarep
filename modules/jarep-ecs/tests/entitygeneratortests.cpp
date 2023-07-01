//
// Created by Sebastian Borsch on 01.07.23.
//
#if __APPLE__
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#include "../src/entitygenerator.hpp"
#endif

TEST_CASE("Add new entity"){
    auto *eg = new EntityGenerator();
    Entity newEntity = eg->createEntity();
    REQUIRE(newEntity == 0);
}
