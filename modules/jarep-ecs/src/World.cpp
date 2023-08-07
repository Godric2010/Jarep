//
// Created by Sebastian Borsch on 01.07.23.
//

#include "World.hpp"
#include "Component.hpp"

class Test : public Component{
    public:
        Test(int i) {
            value = i;
        };

        ~Test() = default;

        int value;
};

Entity World::createNewEntity() {
    auto entity = entityGenerator->createEntity();
    if (!entity.has_value()) return 0;

    auto archetype = Archetype::createEmpty();

    auto target = Archetype::createFromAdd<Test>(archetype);
    auto test = std::make_shared<Test>(Test(2));
    target.value()->setComponentInstance(test);
    auto t_from_a = target.value()->getComponent<Test>(0);



    auto target02 = Archetype::createFromRemove<Test>(target.value());

    return 0;
}

void World::removeEntity() {

}


