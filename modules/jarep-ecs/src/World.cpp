//
// Created by Sebastian Borsch on 01.07.23.
//

#include "../include/World.hpp"

Entity World::createNewEntity() {
    auto entity = entityGenerator->createEntity();
    if(!entity.has_value()) return 0;

    auto archetype = Archetype::createEmpty();
    archetype->entities.push_back(entity.value());

    auto target = Archetype::createFromAdd<int>(archetype);

    return 0;
}
void World::removeEntity() {

}


