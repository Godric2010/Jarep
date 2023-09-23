#if __APPLE__

#include <catch2/catch_test_macros.hpp>
#include <utility>

#else

#include <catch2/catch.hpp>

#endif

#include "../src/world.hpp"
#include <vector>
#include <typeindex>
#include <memory>
#include <optional>

class MyTestComponent : public Component {
	public:
		 MyTestComponent() : Component() {
			myTestValue = 0;
		}

		~MyTestComponent() = default;

		int myTestValue;
};

class MyTestSystem : public System {

	public:
		MyTestSystem() : System() {};

		~MyTestSystem() override = default;

	protected:
		void update() override {

			auto entity = getEntities()[0];
			auto myTestComponent = getComponent<MyTestComponent>(entity);
			if (!myTestComponent.has_value()) {
				std::cout << "Component is missing!" << std::endl;
			}
			myTestComponent.value()->myTestValue++;
			std::cout << "My Test Component new value: " << myTestComponent.value()->myTestValue << std::endl;
		}
};


class WorldFriendAccessor {
	public:
		static bool hasEntityExpectedValues(std::shared_ptr<World> &world, Entity &entityToCheck, bool isAlive,
		                                    Signature expectedSignature, size_t expectedArchetypeIndex) {
			bool aliveStatus = world->entityManager->isAlive(entityToCheck);
			// If the is not alive and the status was expected as such, the test is over at this point, since dead entities contain neither signatures nor archetype indices.
			if (!aliveStatus && !isAlive) return true;

			auto entitySignature = world->entityManager->getSignature(entityToCheck);
			auto entityArchetypeIndex = world->entityManager->getArchetypeIndex(entityToCheck);

			if (!entitySignature.has_value() || !entityArchetypeIndex.has_value()) return false;

			return aliveStatus == isAlive && entitySignature.value() == expectedSignature &&
			       entityArchetypeIndex.value() == expectedArchetypeIndex;
		}

		static bool doesSystemReferesToEntity(std::shared_ptr<World>& world, Entity& entity){
			return world->systemManager->assignedEntitySystemMap.contains(entity);
		}

		static bool doesComponentExist(std::shared_ptr<World>& world, Signature archetypeSignature, std::shared_ptr<MyTestComponent>& testComponent){
			auto availableComponents = world->componentManager->archetypeSignatureMap[archetypeSignature]->getComponentsWithEntities<MyTestComponent>();
			for (const auto &availableComponent: availableComponents) {
				if (availableComponent->myTestValue == testComponent->myTestValue) {
					return true;
				}
			}
			return false;
		}

		static bool isEntitySignatureAndIndexCorrect(std::shared_ptr<World>& world, Entity entity, int expectedTestValue){
			auto signature = world->entityManager->entitySignatureMap[entity];
			auto archetypeIndex = world->entityManager->entityArchetypeIndexMap[entity];

			auto storedComponent = world->componentManager->archetypeSignatureMap[signature]->getComponent<MyTestComponent>(archetypeIndex);
			if(!storedComponent.has_value()) return false;

			return storedComponent.value()->myTestValue == expectedTestValue;
		}

		static bool isEntityAlive(std::shared_ptr<World>& world, Entity& entity){
			return world->entityManager->isAlive(entity);
		}

		static void setEntityCount(std::shared_ptr<World> &world, uint count) {
			world->entityManager->nextId = count;
		}

		static Entity createEmptyEntity(std::shared_ptr<World>& world){
			auto entity = Entity(world->entityManager->nextId);
			world->entityManager->nextId++;
			world->entityManager->entityArchetypeIndexMap[entity] = 0;
			world->entityManager->entitySignatureMap[entity] = Signature(0);
			return entity;
		}

		static void addTestComponentToEntity(std::shared_ptr<World> &world, Entity &entity, std::shared_ptr<MyTestComponent>& component) {
			auto emptyArchetype = Archetype::createEmpty();
			const auto testComponentSignature = Signature(1);
			world->componentManager->componentBitMap[typeid(MyTestComponent)] = testComponentSignature;
			if (!world->componentManager->archetypeSignatureMap.contains(testComponentSignature)) {

				world->componentManager->archetypeSignatureMap[testComponentSignature] = Archetype::createFromAdd<MyTestComponent>(emptyArchetype).value();
			}
			auto newEntityArchetypeIndex = world->componentManager->archetypeSignatureMap[testComponentSignature]->migrateEntity(
					emptyArchetype, entity);
			world->componentManager->archetypeSignatureMap[testComponentSignature]->setComponentInstance(component);

			world->entityManager->assignNewSignature(entity, Signature(1), newEntityArchetypeIndex.value());
		}

		static void addTestEntityToTestSystem(std::shared_ptr<World> &world, Entity& entity){

			if(!world->systemManager->systemTypeIndexMap.contains(typeid(MyTestSystem))){
				world->systemManager->systemTypeIndexMap[typeid(MyTestSystem)] = std::make_unique<MyTestSystem>();
				world->systemManager->systemSignatureMap[typeid(MyTestSystem)] = Signature(1);
			}

			auto assignedSystems = std::vector<std::type_index>();
			assignedSystems.emplace_back(typeid(MyTestSystem));
			world->systemManager->assignedEntitySystemMap[entity] = assignedSystems;

			auto map = std::unordered_map<Entity, std::tuple<Signature, size_t>>();
			map[entity] = std::make_tuple(world->entityManager->entitySignatureMap[entity], world->entityManager->entityArchetypeIndexMap[entity]);
			world->systemManager->setSystemData(typeid(MyTestSystem), map);
		}
};


TEST_CASE("World - Set test name here") {

	auto world = std::make_unique<World>();

	auto myTestSystemRequiredComponents = std::vector<std::type_index>();
	myTestSystemRequiredComponents.emplace_back(typeid(MyTestComponent));

	auto myEntity = world->createNewEntity().value();
	world->addComponent<MyTestComponent>(myEntity);
	world->registerSystem<MyTestSystem>(myTestSystemRequiredComponents);

	for (int i = 0; i < 10; ++i) {
		world->tick();
	}

	REQUIRE(1 == 1);
}

TEST_CASE("World - Add Entities") {
	auto world = std::make_shared<World>();

	SECTION("Add valid entity - Entity is created empty") {
		auto entity = world->createNewEntity();
		REQUIRE(entity.has_value());
		REQUIRE(WorldFriendAccessor::hasEntityExpectedValues(world, entity.value(), true, Signature(0), 0));
	}

	SECTION("Add invalid entity - Entity creation returns nullopt") {
		WorldFriendAccessor::setEntityCount(world, std::numeric_limits<uint>::max());
		auto entity = world->createNewEntity();
		REQUIRE_FALSE(entity.has_value());
	}
}

TEST_CASE("World - Remove Entities") {
	auto world = std::make_shared<World>();
	auto entityA = WorldFriendAccessor::createEmptyEntity(world);
	auto entityB = WorldFriendAccessor::createEmptyEntity(world);
	auto entityC = WorldFriendAccessor::createEmptyEntity(world);

	auto componentA = std::make_shared<MyTestComponent>();
	componentA->myTestValue = 10;
	WorldFriendAccessor::addTestComponentToEntity(world, entityA, componentA);
	WorldFriendAccessor::addTestEntityToTestSystem(world, entityA);

	auto componentB = std::make_shared<MyTestComponent>();
	componentB->myTestValue = 20;
	WorldFriendAccessor::addTestComponentToEntity(world, entityB, componentB);
	WorldFriendAccessor::addTestEntityToTestSystem(world, entityB);

	auto componentC= std::make_shared<MyTestComponent>();
	componentC->myTestValue = 30;
	WorldFriendAccessor::addTestComponentToEntity(world, entityC, componentC);
	WorldFriendAccessor::addTestEntityToTestSystem(world, entityC);

	SECTION("Remove valid entity - System and Component Manager loose connection to entity, all other entities stay valid"){
		world->removeEntity(entityB);
		REQUIRE_FALSE(WorldFriendAccessor::doesComponentExist(world, Signature(1), componentB));
		REQUIRE_FALSE(WorldFriendAccessor::doesSystemReferesToEntity(world, entityB));
		REQUIRE_FALSE(WorldFriendAccessor::isEntityAlive(world,entityB));

		REQUIRE(WorldFriendAccessor::doesComponentExist(world, Signature(1), componentA));
		REQUIRE(WorldFriendAccessor::isEntitySignatureAndIndexCorrect(world, entityA, 10));
		REQUIRE(WorldFriendAccessor::doesSystemReferesToEntity(world, entityA));

		REQUIRE(WorldFriendAccessor::doesComponentExist(world, Signature(1), componentC));
		REQUIRE(WorldFriendAccessor::isEntitySignatureAndIndexCorrect(world, entityC, 30));
		REQUIRE(WorldFriendAccessor::doesSystemReferesToEntity(world, entityC));
	}

}