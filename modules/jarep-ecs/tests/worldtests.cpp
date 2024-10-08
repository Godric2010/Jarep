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

class MyInvalidTestComponent : public Component{

	public :
		MyInvalidTestComponent() : Component() {
			myTestValue = 900;
		}
		~MyInvalidTestComponent() = default;

		int myTestValue;
};

class MyTestSystem : public System {

	public:
		MyTestSystem() : System() {};

		~MyTestSystem() override = default;

		std::unordered_map<Entity, std::optional<std::shared_ptr<MyTestComponent>>> getEntityWithComponentReference() {
			auto map = std::unordered_map<Entity, std::optional<std::shared_ptr<MyTestComponent>>>();
			for (const auto &entity: getEntities()) {
				map[entity] = getComponent<MyTestComponent>(entity);
			}
			return map;
		}

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

			auto signature = entitySignature.value();
			auto archetypeIndex= entityArchetypeIndex.value();

			return aliveStatus == isAlive && signature == expectedSignature &&
			       archetypeIndex == expectedArchetypeIndex;
		}

		static bool doesSystemReferesToEntity(std::shared_ptr<World> &world, Entity &entity) {
			return world->systemManager->assignedEntitySystemMap.contains(entity);
		}

		static bool doesComponentExist(std::shared_ptr<World> &world, Signature archetypeSignature,
		                               std::shared_ptr<MyTestComponent> &testComponent) {
			auto availableComponents = world->componentManager->archetypeSignatureMap[archetypeSignature]->getComponentsWithEntities<MyTestComponent>();
			for (const auto &availableComponent: availableComponents) {
				if (availableComponent->myTestValue == testComponent->myTestValue) {
					return true;
				}
			}
			return false;
		}

		static void registerTestComponent(std::shared_ptr<World> &world) {
			world->componentManager->registerComponent<MyTestComponent>();
		}

		static void setTestComponentValue(std::shared_ptr<World> &world, int newValue) {
			world->componentManager->archetypeSignatureMap[Signature(1)]->getComponent<MyTestComponent>(
					0).value()->myTestValue = newValue;
		}

		static bool
		isEntitySignatureAndIndexCorrect(std::shared_ptr<World> &world, Entity entity, int expectedTestValue) {
			auto signature = world->entityManager->entitySignatureMap[entity];
			auto archetypeIndex = world->entityManager->entityArchetypeIndexMap[entity];

			auto storedComponent = world->componentManager->archetypeSignatureMap[signature]->getComponent<MyTestComponent>(
					archetypeIndex);
			if (!storedComponent.has_value()) return false;

			return storedComponent.value()->myTestValue == expectedTestValue;
		}

		static bool isEntityAlive(std::shared_ptr<World> &world, Entity &entity) {
			return world->entityManager->isAlive(entity);
		}

		static void setEntityCount(std::shared_ptr<World> &world, uint count) {
			world->entityManager->nextId = count;
		}

		static Entity createEmptyEntity(std::shared_ptr<World> &world) {
			auto entity = Entity(world->entityManager->nextId);
			world->entityManager->nextId++;
			world->entityManager->entityArchetypeIndexMap[entity] = 0;
			world->entityManager->entitySignatureMap[entity] = Signature(0);
			return entity;
		}

		static void createTestSystem(std::shared_ptr<World> &world) {
			auto getComponentFunc = std::make_shared<GetComponentsFunc>(world->componentManager);

			if (world->systemManager->systemTypeIndexMap.contains(typeid(MyTestSystem))) {
				return;
			}

			auto system = std::make_unique<MyTestSystem>();
			system->getComponentFunc = getComponentFunc;
			world->systemManager->systemTypeIndexMap[typeid(MyTestSystem)] = std::move(system);
			world->systemManager->systemSignatureMap[typeid(MyTestSystem)] = Signature(1);

		}

		static void assignEntityToSystem(std::shared_ptr<World>& world, Entity entity){
			auto map = std::unordered_map<Entity, std::tuple<Signature, size_t>>();
			map[entity] = std::make_tuple(Signature(1), 0);
			world->systemManager->addEntitiesToSystem(typeid(MyTestSystem), map);
		}

		static MyTestSystem *getTestSystem(std::shared_ptr<World> &world) {
			System &system = *world->systemManager->systemTypeIndexMap[typeid(MyTestSystem)].get();
			auto *mySystem = dynamic_cast<MyTestSystem *>(&system);
			return mySystem;
		}

		static bool isTestSystemNotRegistered(std::shared_ptr<World> &world) {
			if (world->systemManager->isSystemRegistred(typeid(MyTestSystem))) {
				return false;
			}

			return world->systemManager->assignedEntitySystemMap.empty() &&
			       world->systemManager->systemSignatureMap.empty();
		}

		static void addTestComponentToEntity(std::shared_ptr<World> &world, Entity &entity,
		                                     std::shared_ptr<MyTestComponent> &component) {
			auto emptyArchetype = Archetype::createEmpty();
			const auto testComponentSignature = Signature(1);
			world->componentManager->componentBitMap[typeid(MyTestComponent)] = testComponentSignature;
			if (!world->componentManager->archetypeSignatureMap.contains(testComponentSignature)) {

				world->componentManager->archetypeSignatureMap[testComponentSignature] = Archetype::createFromAdd<MyTestComponent>(
						emptyArchetype).value();
			}
			auto newEntityArchetypeIndex = world->componentManager->archetypeSignatureMap[testComponentSignature]->migrateEntity(
					emptyArchetype, entity);
			world->componentManager->archetypeSignatureMap[testComponentSignature]->setComponentInstance(component);

			world->entityManager->assignNewSignature(entity, Signature(1), newEntityArchetypeIndex.value());
		}

		static void addTestEntityToTestSystem(std::shared_ptr<World> &world, Entity &entity) {

			if (!world->systemManager->systemTypeIndexMap.contains(typeid(MyTestSystem))) {
				world->systemManager->systemTypeIndexMap[typeid(MyTestSystem)] = std::make_unique<MyTestSystem>();
				world->systemManager->systemSignatureMap[typeid(MyTestSystem)] = Signature(1);
			}

			auto assignedSystems = std::vector<std::type_index>();
			assignedSystems.emplace_back(typeid(MyTestSystem));
			world->systemManager->assignedEntitySystemMap[entity] = assignedSystems;

			auto map = std::unordered_map<Entity, std::tuple<Signature, size_t>>();
			map[entity] = std::make_tuple(world->entityManager->entitySignatureMap[entity],
			                              world->entityManager->entityArchetypeIndexMap[entity]);
			world->systemManager->addEntitiesToSystem(typeid(MyTestSystem), map);
		}
};

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

	auto componentC = std::make_shared<MyTestComponent>();
	componentC->myTestValue = 30;
	WorldFriendAccessor::addTestComponentToEntity(world, entityC, componentC);
	WorldFriendAccessor::addTestEntityToTestSystem(world, entityC);

	SECTION("Remove valid entity - System and Component Manager loose connection to entity, all other entities stay valid") {
		world->removeEntity(entityB);
		REQUIRE_FALSE(WorldFriendAccessor::doesComponentExist(world, Signature(1), componentB));
		REQUIRE_FALSE(WorldFriendAccessor::doesSystemReferesToEntity(world, entityB));
		REQUIRE_FALSE(WorldFriendAccessor::isEntityAlive(world, entityB));

		REQUIRE(WorldFriendAccessor::doesComponentExist(world, Signature(1), componentA));
		REQUIRE(WorldFriendAccessor::isEntitySignatureAndIndexCorrect(world, entityA, 10));
		REQUIRE(WorldFriendAccessor::doesSystemReferesToEntity(world, entityA));

		REQUIRE(WorldFriendAccessor::doesComponentExist(world, Signature(1), componentC));
		REQUIRE(WorldFriendAccessor::isEntitySignatureAndIndexCorrect(world, entityC, 30));
		REQUIRE(WorldFriendAccessor::doesSystemReferesToEntity(world, entityC));
	}
}

TEST_CASE("World - Add Component") {

	auto world = std::make_shared<World>();
	auto entityA = WorldFriendAccessor::createEmptyEntity(world);
	auto invalidEntity = Entity(12);

	WorldFriendAccessor::createTestSystem(world);

	SECTION("Add component - Entity, component and system manager get updated") {

		world->addComponent<MyTestComponent>(entityA);

		REQUIRE(WorldFriendAccessor::hasEntityExpectedValues(world, entityA, true, Signature(1), 0));

		MyTestSystem *system = WorldFriendAccessor::getTestSystem(world);
		auto entityComponentRefs = system->getEntityWithComponentReference();
		auto componentOfEntityA = entityComponentRefs[entityA];
		REQUIRE(componentOfEntityA.has_value());
		REQUIRE(componentOfEntityA.value()->myTestValue == 0);
	}

	SECTION("Add same component twice to same entity - Only once component gets assigned to entity and system") {
		world->addComponent<MyTestComponent>(entityA);

		REQUIRE(WorldFriendAccessor::hasEntityExpectedValues(world, entityA, true, Signature(1), 0));

		WorldFriendAccessor::setTestComponentValue(world, 42);
		world->addComponent<MyTestComponent>(entityA);
		auto *system = const_cast<MyTestSystem *>(WorldFriendAccessor::getTestSystem(world));
		auto entityComponentRefs = system->getEntityWithComponentReference();
		auto componentOfEntityA = entityComponentRefs[entityA];
		REQUIRE(componentOfEntityA.has_value());
		REQUIRE(componentOfEntityA.value()->myTestValue == 42);
	}

	SECTION("Add component to invalid entity - No component gets added to anything") {
		REQUIRE_THROWS(world->addComponent<MyTestComponent>(invalidEntity));

		auto *system = const_cast<MyTestSystem *>(WorldFriendAccessor::getTestSystem(world));
		auto entityComponentRefs = system->getEntityWithComponentReference();
		REQUIRE(entityComponentRefs.empty());
	}
}

TEST_CASE("World - Remove Component") {
	auto world = std::make_shared<World>();
	auto entityA = WorldFriendAccessor::createEmptyEntity(world);
	auto testComponentToRemove = std::make_shared<MyTestComponent>();
	WorldFriendAccessor::addTestComponentToEntity(world, entityA, testComponentToRemove);
	WorldFriendAccessor::createTestSystem(world);
	WorldFriendAccessor::assignEntityToSystem(world, entityA);

	SECTION("Remove component - Entity, component and system manager get updated") {

		REQUIRE_NOTHROW(world->removeComponent<MyTestComponent>(entityA));

		REQUIRE(WorldFriendAccessor::hasEntityExpectedValues(world,entityA,true, Signature(0), 0));
		auto *system = const_cast<MyTestSystem *>(WorldFriendAccessor::getTestSystem(world));
		auto entityComponentRefs = system->getEntityWithComponentReference();
		auto componentOfEntityA = entityComponentRefs[entityA];
		REQUIRE_FALSE(componentOfEntityA.has_value());
	}

	SECTION("Remove component from invalid entity - No component will be removed") {
		auto invalidEntity = Entity(12);
		REQUIRE_THROWS(world->removeComponent<MyTestComponent>(invalidEntity));

		REQUIRE(WorldFriendAccessor::hasEntityExpectedValues(world,entityA,true, Signature(1), 0));
		auto *system = const_cast<MyTestSystem *>(WorldFriendAccessor::getTestSystem(world));
		auto entityComponentRefs = system->getEntityWithComponentReference();
		auto componentOfEntityA = entityComponentRefs[entityA];
		REQUIRE(componentOfEntityA.has_value());
	}

	SECTION("Remove invalid component from entity - No component will be removed") {
		auto invalidComponent = std::make_shared<MyInvalidTestComponent>();

		REQUIRE_NOTHROW(world->removeComponent<MyInvalidTestComponent>(entityA));

		REQUIRE(WorldFriendAccessor::hasEntityExpectedValues(world,entityA,true, Signature(1), 0));
		auto *system = const_cast<MyTestSystem *>(WorldFriendAccessor::getTestSystem(world));
		auto entityComponentRefs = system->getEntityWithComponentReference();
		auto componentOfEntityA = entityComponentRefs[entityA];
		REQUIRE(componentOfEntityA.has_value());
	}

}

TEST_CASE("World - Add System") {
	auto world = std::make_shared<World>();

	SECTION("Add system first - Registration works with no issues") {

		auto requiredComponents = std::vector<std::type_index>();
		requiredComponents.emplace_back(typeid(MyTestComponent));

		WorldFriendAccessor::registerTestComponent(world);

		REQUIRE_NOTHROW(world->registerSystem<MyTestSystem>(requiredComponents));

		auto testSystem = WorldFriendAccessor::getTestSystem(world);
		auto entityComponentRefs = testSystem->getEntityWithComponentReference();
		REQUIRE(entityComponentRefs.empty());
	}

	SECTION("Add system first - Registration but the required components are not registered") {
		auto requiredComponents = std::vector<std::type_index>();
		requiredComponents.emplace_back(typeid(MyTestComponent));
		REQUIRE_THROWS(world->registerSystem<MyTestSystem>(requiredComponents));
	}

	SECTION("Add system last - Entities and components get registered correctly on the system") {

		auto entity = WorldFriendAccessor::createEmptyEntity(world);
		auto component = std::make_shared<MyTestComponent>();
		component->myTestValue = 42;
		WorldFriendAccessor::addTestComponentToEntity(world, entity, component);

		auto requiredComponents = std::vector<std::type_index>();
		requiredComponents.emplace_back(typeid(MyTestComponent));
		REQUIRE_NOTHROW(world->registerSystem<MyTestSystem>(requiredComponents));

		auto testSystem = WorldFriendAccessor::getTestSystem(world);
		auto entityComponentRefs = testSystem->getEntityWithComponentReference();
		REQUIRE(entityComponentRefs.contains(entity));
		REQUIRE(entityComponentRefs[entity].value()->myTestValue == 42);
	}

	SECTION("Add system twice - System gets only registered once") {
		auto entity = WorldFriendAccessor::createEmptyEntity(world);
		auto component = std::make_shared<MyTestComponent>();
		component->myTestValue = 42;
		WorldFriendAccessor::addTestComponentToEntity(world, entity, component);

		auto requiredComponents = std::vector<std::type_index>();
		requiredComponents.emplace_back(typeid(MyTestComponent));
		REQUIRE(world->registerSystem<MyTestSystem>(requiredComponents));

		REQUIRE_FALSE(world->registerSystem<MyTestSystem>(requiredComponents));
	}
}

TEST_CASE("World - Remove System") {
	auto world = std::make_shared<World>();
	auto entity = WorldFriendAccessor::createEmptyEntity(world);
	auto component = std::make_shared<MyTestComponent>();
	component->myTestValue = 42;
	WorldFriendAccessor::addTestComponentToEntity(world, entity, component);
	WorldFriendAccessor::createTestSystem(world);

	SECTION("Remove system - Entities and Components get removed from associations") {

		world->deregisterSystem<MyTestSystem>();
		REQUIRE(WorldFriendAccessor::isTestSystemNotRegistered(world));
	}

	SECTION("Remove system twice - Second call does not break the ecs") {
		REQUIRE_NOTHROW(world->deregisterSystem<MyTestSystem>());
	}

}