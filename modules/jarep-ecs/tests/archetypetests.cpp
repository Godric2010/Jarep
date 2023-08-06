//
// Created by Sebastian Borsch on 13.07.23.
//

#if __APPLE__

#include <catch2/catch_test_macros.hpp>
#include <utility>

#else
#include <catch2/catch.hpp>
#endif

#include "../src/archetype.hpp"

class ComponentA {

    public:
        ComponentA() = default;

        ~ComponentA() = default;
};

class ComponentB {
    public:
        ComponentB(int initialValue, std::string initialText) {
            value = initialValue;
            text = std::move(initialText);
        };

        ~ComponentB() = default;

        int value;
        std::string text;
};

TEST_CASE("Archetype - Create an empty Archetype and add new components and remove them.") {

    // Create an empty archetype
    auto archetype01 = Archetype::createEmpty();

    // Add one component to it, thus creating a new archetype
    auto archetype02 = Archetype::createFromAdd<ComponentA>(archetype01);
    REQUIRE(archetype02.has_value());
    REQUIRE(archetype02.value()->containsType<ComponentA>());
    REQUIRE_FALSE(archetype02.value()->containsType<ComponentB>());

    // Remove the component from the archetype before, thus having an empty archetype again
    auto archetype03_opt = Archetype::createFromRemove<ComponentA>(archetype02.value());
    REQUIRE(archetype03_opt.has_value());
    REQUIRE_FALSE(archetype03_opt.value()->containsType<ComponentA>());

}

TEST_CASE("Archetype - Create an archetype with a data component, access the data and remove it through its entity" ) {

    /// Setup the empty archetype
    auto archetype_empty = Archetype::createEmpty();

    /// Setup the component
    auto myComponent = std::make_shared<ComponentB>(ComponentB(1, "Hello World!"));

    auto test_archetype = Archetype::createFromAdd<ComponentB>(archetype_empty);
    REQUIRE(test_archetype.has_value());

    /// Migrate and test the component instance adding and reading
    test_archetype.value()->migrateEntity(archetype_empty, 0);
    test_archetype.value()->setComponentInstance(myComponent);
    auto component = test_archetype.value()->getComponent<ComponentB>(0);
    REQUIRE(component.has_value());
    REQUIRE(component.value()->value == 1);
    REQUIRE(component.value()->text == "Hello World!");

    component.value()->value = 10;
    component = test_archetype.value()->getComponent<ComponentB>(0);
    REQUIRE(component.value()->value == 10);

    /// Create more entities and component instances to add
    auto myComponent2 = std::make_shared<ComponentB>(ComponentB(3, "Bye bye, World!"));
    test_archetype.value()->setComponentInstance(myComponent2);

    /// Get all entities in this archetype with their component instances
    auto components = test_archetype.value()->getComponentsWithEntities<ComponentB>();
    REQUIRE(components.size() == 2);
    REQUIRE(components.at(0)->value == 10);
    REQUIRE(components.at(1)->value == 3);
    REQUIRE(components.at(0)->text == "Hello World!");
    REQUIRE(components.at(1)->text == "Bye bye, World!");

    /// Remove an entity and all of its respected components
    test_archetype.value()->removeComponentsAtEntityIndex(0);
    REQUIRE(test_archetype.value()->getComponentsWithEntities<ComponentB>().size() == 1);
    REQUIRE(test_archetype.value()->getComponent<ComponentB>(0).value()->value == 3);
}
