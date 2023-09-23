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

class EntityManagerTestFriend{
	public:
		static void setEntityCount(std::shared_ptr<EntityManager>& entityManager, uint count){
			entityManager->nextId = count;
		}
};


TEST_CASE("Entity Manager - Creating entities") {
	auto entityManager = std::make_shared<EntityManager>();

	SECTION("Creating one new entity - Has value 0") {
		auto newEntity = entityManager->createEntity();
		REQUIRE(newEntity.has_value());
		REQUIRE(newEntity.value() == 0);
	}

	SECTION("Create the max amount of entities + 1 - Last entity has no value") {
		EntityManagerTestFriend::setEntityCount(entityManager, std::numeric_limits<uint>::max());
		REQUIRE_FALSE(entityManager->createEntity().has_value());
	}
}

TEST_CASE("Entity Manager - Removing entities") {
	auto entityManager = std::make_shared<EntityManager>();
	auto entities = std::vector<Entity>();
	for (int i = 0; i < 3; ++i) {
		entities.push_back(entityManager->createEntity().value());
	}

	SECTION("Remove existing entity - Entity label is 'dead'")
	{
		entityManager->removeEntity(entities[1]);
		REQUIRE_FALSE(entityManager->isAlive(entities[1]));
		REQUIRE(entityManager->isAlive(entities[0]));
		REQUIRE(entityManager->isAlive(entities[2]));
	}

	SECTION("Remove not existing entity - Exception shall rise!") {
		auto invalidEntity = Entity(8);
		REQUIRE_THROWS(entityManager->removeEntity(invalidEntity));
	}

	SECTION("Remove valid entity twice - Entity stays 'dead'") {
		entityManager->removeEntity(entities[1]);
		entityManager->removeEntity(entities[1]);
		REQUIRE_FALSE(entityManager->isAlive(entities[1]));
	}
}

TEST_CASE("Entity Manager - Dead switch tests"){
	auto entityManager = std::make_shared<EntityManager>();
	auto entities = std::vector<Entity>();

	SECTION("Create three entities, delete the middle one and add a fourth entity - Last entity has index 1"){
		for (int i = 0; i < 3; ++i) {
			entities.push_back(entityManager->createEntity().value());
		}

		REQUIRE(entities[2] == 2);

		entityManager->removeEntity(entities[1]);
		auto fourthEntity = entityManager->createEntity();
		REQUIRE(fourthEntity.has_value());
		REQUIRE(fourthEntity.value() == 1);
	}

	SECTION("Create max amount of entities, delete one and add one again - The new entity is valid"){
			EntityManagerTestFriend::setEntityCount(entityManager, std::numeric_limits<uint>::max());

		auto testEntity = Entity(123);
		entityManager->removeEntity(testEntity);
		REQUIRE_FALSE(entityManager->isAlive(testEntity));

		auto newEntity = entityManager->createEntity();
		REQUIRE(newEntity.has_value());
		REQUIRE(newEntity.value() == testEntity);
	}
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
	REQUIRE_FALSE(entityManager->getSignature(entity02).has_value());
}
