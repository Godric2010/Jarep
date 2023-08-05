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
#include <tuple>

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

TEST_CASE("Archetype - Create an empty Archetype and add new components and remove them.","[.]") {

    auto entity = 2;
    auto archetype01 = Archetype::createEmpty();
    archetype01->entities.push_back(entity);

    REQUIRE(archetype01->entities.size() == 1);
    REQUIRE(archetype01->entities[0] == entity);

    auto archetype02 = Archetype::createFromAdd<ComponentA>(archetype01);
    REQUIRE(archetype02.has_value());
    REQUIRE(archetype02.value()->containsType<ComponentA>());
    REQUIRE_FALSE(archetype02.value()->containsType<ComponentB>());

    archetype02.value()->migrateEntity(archetype01, 0);
    REQUIRE(archetype02.value()->entities[0] == 0);
    REQUIRE(archetype01->entities.empty());

    auto archetype03_opt = Archetype::createFromRemove<ComponentA>(archetype02.value());
    REQUIRE(archetype03_opt.has_value());
    REQUIRE_FALSE(archetype03_opt.value()->containsType<ComponentA>());
}

TEST_CASE("Archetype - Create an archetype with a data component, access the data and remove it through its entity", "[.]") {

    /// Setup the empty archetype
    Entity entity = 2;
    auto archetype_empty = Archetype::createEmpty();
    archetype_empty->entities.push_back(entity);

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
    Entity entity2 = 6;
    test_archetype.value()->entities.push_back(entity2);
    auto myComponent2 = std::make_shared<ComponentB>(ComponentB(3, "Bye bye, World!"));
    test_archetype.value()->setComponentInstance(myComponent2);

    /// Get all entities in this archetype with their component instances
    auto entities_and_components = test_archetype.value()->getComponentsWithEntities<ComponentB>();
    REQUIRE(entities_and_components.has_value());
    REQUIRE(std::get<0>(entities_and_components.value().at(0))->value == 10);
    REQUIRE(std::get<0>(entities_and_components.value().at(1))->value == 3);
    REQUIRE(std::get<0>(entities_and_components.value().at(0))->text == "Hello World!");
    REQUIRE(std::get<0>(entities_and_components.value().at(1))->text == "Bye bye, World!");
    REQUIRE(std::get<1>(entities_and_components.value().at(0)) == entity);
    REQUIRE(std::get<1>(entities_and_components.value().at(1)) == entity2);

    /// Remove an entity and all of its respected components
    test_archetype.value()->removeEntity(entity);
    REQUIRE(test_archetype.value()->entities.size()== 1);
    REQUIRE(test_archetype.value()->entities.at(0) == entity2);
    REQUIRE(test_archetype.value()->getComponentsWithEntities<ComponentB>().value().size() == 1);
    REQUIRE(test_archetype.value()->getComponent<ComponentB>(0).value()->value == 3);
}

TEST_CASE("Archetype - Compare hashes for created and anticipated archetypes"){
    auto archetype01 = Archetype::createEmpty();
    auto check_archetype = Archetype::createFromAdd<ComponentA>(archetype01);

    REQUIRE_FALSE(archetype01->getHashValue() == check_archetype.value()->getHashValue());
    REQUIRE(Archetype::generateExpectedHash<ComponentA>(archetype01) == check_archetype.value()->getHashValue());
}

