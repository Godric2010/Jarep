#if __APPLE__

#include <catch2/catch_test_macros.hpp>
#include <utility>

#else
#include <catch2/catch.hpp>
#endif

#include <memory>
#include <tuple>
#include "../src/Component.hpp"
#include "../src/componentmanager.hpp"
#include "../src/signature.hpp"

class ComponentA : public Component {
	public:
		ComponentA() {
			value = 0;
		};

		~ComponentA() = default;

		int value;

};

class ComponentB : public Component {
	public:
		ComponentB() {
			value = 0.0f;
		}

		~ComponentB() = default;

		float value;
};

static Signature componentASignature = Signature(1);
static Signature componentBSignature = Signature(2);

TEST_CASE(
		"ComponentManager - Add component to Signature and get its value, as well as remove one and ensure that the indices stay correct") {

	auto componentA = std::make_shared<ComponentA>();
	componentA->value = 3;

	auto componentB = std::make_shared<ComponentB>();
	componentB->value = 4.5f;

	auto componentBTwo = std::make_shared<ComponentB>();
	componentBTwo->value = 6.3f;

	ComponentManager componentManager;
	componentManager.registerComponent<ComponentA>();
	componentManager.registerComponent<ComponentB>();

	auto componentA1 = componentManager.addComponentToSignature<ComponentA>(Signature(0), 0, componentA);
	REQUIRE(componentA1.has_value());
	REQUIRE(componentA1.value().first == componentASignature);

	auto componentB1 = componentManager.addComponentToSignature<ComponentB>(Signature(0), 0, componentB);
	REQUIRE(componentB1.has_value());
	REQUIRE(componentB1.value().first == componentBSignature);
	REQUIRE(componentB1.value().second == 0);

	auto componentB2 = componentManager.addComponentToSignature<ComponentB>(Signature(0), 1, componentBTwo);
	REQUIRE(componentB2.has_value());
	REQUIRE(componentB2.value().first == componentBSignature);
	REQUIRE(componentB2.value().second == 1);

	auto componentAResult = componentManager.getComponent<ComponentA>(componentA1.value().first, 0);
	REQUIRE(componentAResult.has_value());
	REQUIRE(componentAResult.value()->value == 3);

	std::optional<std::vector<std::tuple<std::shared_ptr<ComponentB>, Signature, size_t>>> componentBResults =
			componentManager.getComponentsOfType<ComponentB>();
	REQUIRE(componentBResults.has_value());
	REQUIRE(componentBResults.value().size() == 2);
	REQUIRE(std::get<0>(componentBResults.value()[0])->value == 4.5f);
	REQUIRE(std::get<0>(componentBResults.value()[1])->value == 6.3f);

	componentManager.removeEntityComponents(componentBSignature, 0);
	componentBResults = componentManager.getComponentsOfType<ComponentB>();
	REQUIRE(componentBResults.has_value());
	REQUIRE(componentBResults.value().size() == 1);
	REQUIRE(std::get<0>(componentBResults.value()[0])->value == 6.3f);
}

TEST_CASE("ComponentManager - Try to register the same Component multiple times") {

	ComponentManager componentManager;
	componentManager.registerComponent<ComponentA>();
	componentManager.registerComponent<ComponentA>();

	auto componentA = std::make_shared<ComponentA>();
	componentA->value = 42;

	auto componentACM = componentManager.addComponentToSignature<ComponentA>(Signature(0), 0, componentA);
	REQUIRE(componentACM.has_value());
	REQUIRE(componentACM.value().first == componentASignature);
}

TEST_CASE("ComponentManager - Add multiple components and than remove one from") {

	ComponentManager componentManager;
	componentManager.registerComponent<ComponentA>();
	componentManager.registerComponent<ComponentB>();

	auto a = std::make_shared<ComponentA>();
	a->value = 42;

	auto b = std::make_shared<ComponentB>();
	b->value = 4.2f;

	auto step1 = componentManager.addComponentToSignature<ComponentA>(Signature(0), 0, a);
	auto step2 = componentManager.addComponentToSignature<ComponentB>(componentASignature, 0, b);

	REQUIRE(step2.has_value());
	REQUIRE(step2.value().first == (componentASignature | componentBSignature));

	auto step3 = componentManager.removeComponentFromSignature<ComponentA>((componentASignature | componentBSignature), 0);
	REQUIRE(step3.has_value());
	REQUIRE(step3.value().first == componentBSignature);
}