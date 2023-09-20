//
// Created by Sebastian Borsch on 01.07.23.
//
#if __APPLE__
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif

#include "../src/entitymanager.hpp"
#include <memory>

TEST_CASE("Entity Manager - Add new entity - Entity is 0") {
    auto *eg = new EntityManager();
    auto newEntity = eg->createEntity();
    REQUIRE(newEntity.has_value());
    REQUIRE(newEntity.value() == 0);
}

TEST_CASE("Entity Manager - Add two entities, remove one and add one again")
{
	auto entityManager = std::make_shared<EntityManager>();
	auto entity01 = entityManager->createEntity().value();
	auto entity02 = entityManager->createEntity();

	entityManager->removeEntity(entity01);

	auto entity03 = entityManager->createEntity();
	REQUIRE(entity03.has_value());
	REQUIRE(entity03.value() == 0);
	REQUIRE(entity02.value() == 1);

}

TEST_CASE("Entity Manager - Remove an existing entity - Entity should be labeled as dead"){
    auto *eg = new EntityManager();
    auto entity_opt = eg->createEntity();
    REQUIRE(entity_opt.has_value());
    auto entity = entity_opt.value();
    eg->removeEntity(entity);
    REQUIRE(eg->isAlive(entity) == false);
}

TEST_CASE("Entity Manager - Remove an invalid entity - Expected Exception"){
    auto *eg = new EntityManager();
    Entity entity = 15;

    REQUIRE_THROWS(eg->removeEntity(entity));
}

TEST_CASE("Entity Manager - Remove already dead entity - Nothing happens"){
    auto *eg = new EntityManager();
    auto entity_opt = eg->createEntity();
    REQUIRE(entity_opt.has_value());
    auto entity = entity_opt.value();
    eg->removeEntity(entity);
    REQUIRE_NOTHROW(eg->removeEntity(entity));

}

TEST_CASE("Entity Manager - Assign new Signature - Signature is available as long as entity lives")
{
	auto entityManager = std::make_shared<EntityManager>();
	auto entity01 = entityManager->createEntity().value();

	auto signature = Signature(21);

	entityManager->assignNewSignature(entity01, signature, 0);
	REQUIRE(entityManager->getSignature(entity01) == signature);

	entityManager->removeEntity(entity01);
	REQUIRE(entityManager->getSignature(entity01) == std::nullopt);

	auto entity02 = entityManager->createEntity().value();
	REQUIRE_FALSE(entityManager->getSignature(entity02).has_value() );
}
