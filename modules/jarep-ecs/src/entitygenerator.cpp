//
// Created by Sebastian Borsch on 01.07.23.
//

#include "entitygenerator.hpp"

EntityGenerator::EntityGenerator() {
    nextId = 0;
    deadEntities.clear();

}

EntityGenerator::~EntityGenerator() {
    nextId = 0;
    deadEntities.clear();
}

Entity EntityGenerator::createEntity() {
    Entity newEntity = nextId;

    if(nextId == sizeof(size_t) + 1){
        printf("Exceeded the maximum entities!");
        return -1;
    }

    nextId += 1;
    return newEntity;
}

void EntityGenerator::removeEntity(Entity entity) {

    if(!isAlive(entity)) return;
    deadEntities.insert(entity);

}

bool EntityGenerator::isAlive(Entity entity) {
    if(entity >= nextId) {
        throw std::runtime_error("Requesting alive status for uninitialized entities is forbidden!");
    }

    auto iterator = deadEntities.find(entity);
    return iterator != deadEntities.end();
}
