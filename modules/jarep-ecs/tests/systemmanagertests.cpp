//
// Created by sebastian on 10.09.23.
//
#if __APPLE__
#include <catch2/catch_test_macros.hpp>
#include <utility>

#else

#include <catch2/catch.hpp>

#endif

#include "systemmanager.hpp"

class TestSystemA : public System {
	public:
		TestSystemA() : System() {
			systemCalls = 0;
		};
		~TestSystemA() override = default;
		int systemCalls;

	protected:
		void update() override {
			systemCalls++;
		}
};

class TestSystemB : public System{
	public:
		TestSystemB() : System(){}
		~TestSystemB() override = default;

		std::vector<Entity> getEntitiesForTest(){
			return  getEntities();
		}

	protected:
		void update() override{

		}
};

TEST_CASE("System Manager") {
	auto systemManager = std::make_unique<SystemManager>();

	SECTION("Register system once - Registration successful"){
		auto result = systemManager->registerSystem<TestSystemA>(Signature(0), nullptr);
		REQUIRE(result.has_value());
	}

	systemManager->registerSystem<TestSystemB>(Signature(0), nullptr);

	SECTION("Register system multiple times - Only one registration should work"){
		for (int i = 0; i < 4; ++i) {
			auto result = systemManager->registerSystem<TestSystemA>(Signature(0), nullptr);
			if(i == 0){
				REQUIRE(result.has_value());
				continue;
			}
			REQUIRE_FALSE(result.has_value());
		}
	}

	SECTION("Update with one registered system - System gets updated"){
		systemManager->registerSystem<TestSystemA>(Signature(0), nullptr);
		systemManager->update();
		auto systemResult = systemManager->getSystem(typeid(TestSystemA));
		REQUIRE(systemResult.has_value());
		auto systemInstance = dynamic_cast<const TestSystemA*>(systemResult.value());
		REQUIRE(systemInstance->systemCalls == 1);
	}

	SECTION("Deregister system - system is not available anymore"){
		systemManager->unregisterSystem<TestSystemB>();
		auto systemResult = systemManager->getSystem(typeid(TestSystemB));
		REQUIRE_FALSE(systemResult.has_value());
	}

	SECTION("Deregister not existing system - Nothing happens"){
		systemManager->unregisterSystem<TestSystemA>();
		auto systemResult = systemManager->getSystem(typeid(TestSystemA));
		REQUIRE_FALSE(systemResult.has_value());
		auto otherSystemResult = systemManager->getSystem(typeid(TestSystemB));
		REQUIRE(otherSystemResult.has_value());
	}

	SECTION("Update system data of valid system - New system data is set"){
		auto newTestData = std::unordered_map<Entity, std::tuple<Signature, size_t>>();
		newTestData[Entity(12)] = std::make_tuple(Signature(1), 1);
		systemManager->updateSystemData(typeid(TestSystemB), newTestData);
		auto result = systemManager->getSystem(typeid(TestSystemB));
		REQUIRE(result.has_value());
		auto testSystem = dynamic_cast<TestSystemB *>(result.value());
		REQUIRE(testSystem->getEntitiesForTest().size() == 1);
		REQUIRE(testSystem->getEntitiesForTest()[0] == Entity(12));
	}

	SECTION("Update system data of not registered system - Nothing happens"){
		auto newTestData = std::unordered_map<Entity, std::tuple<Signature, size_t>>();
		newTestData[Entity(12)] = std::make_tuple(Signature(1), 1);
		systemManager->updateSystemData(typeid(TestSystemA), newTestData);
		auto result = systemManager->getSystem(typeid(TestSystemA));
		REQUIRE_FALSE(result.has_value());

	}
}