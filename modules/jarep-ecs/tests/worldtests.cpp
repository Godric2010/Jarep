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

		int myTestValue{};
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


TEST_CASE("World - Set test name here") {

	auto world = std::make_unique<World>();

	auto myTestSystemRequiredComponents = std::vector<std::type_index>();
	myTestSystemRequiredComponents.emplace_back(typeid(MyTestComponent));

	auto myEntity = world->createNewEntity();
	world->addComponent<MyTestComponent>(myEntity);
	world->registerSystem<MyTestSystem>(myTestSystemRequiredComponents);

	for (int i = 0; i < 10; ++i) {
		world->tick();
	}

	REQUIRE(1 == 1);
} 