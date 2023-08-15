#if __APPLE__

#include <catch2/catch_test_macros.hpp>
#include <utility>

#else
#include <catch2/catch.hpp>
#endif

#include "../src/world.hpp"
#include "../src/system.hpp"
#include "../src/component.hpp"
#include <vector>
#include <typeindex>

class MyTestComponent: public Component{
	public:
		MyTestComponent() : Component(){
			myTestValue = 0;
		}
		~MyTestComponent() = default;

		int myTestValue{};
};

class MyTestSystem : public System{

	public:
		MyTestSystem(std::vector<std::type_index> requiredComponentTypes) : System(std::move(requiredComponentTypes)){}
		~MyTestSystem() override = default;

	protected:
		void update() override{

		}
};



TEST_CASE("world - Set test name here"){

	auto world = std::make_unique<World>();

	auto myTestSystemRequiredComponents = std::vector<std::type_index>();
	myTestSystemRequiredComponents.emplace_back(typeid(MyTestComponent));

	auto myEntity = world->createNewEntity();
	world->addComponent<MyTestComponent>(myEntity);
	world->registerSystem<MyTestSystem>(myTestSystemRequiredComponents);

	REQUIRE(1 == 1);
} 