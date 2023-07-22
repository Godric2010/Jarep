//
// Created by Sebastian Borsch on 01.07.23.
//

#ifndef JAREP_ENTITY_GENERATOR_HPP
#define JAREP_ENTITY_GENERATOR_HPP

#include <iostream>
#include <unordered_set>
#include <optional>

typedef size_t Entity;

class EntityGenerator {
    public:
        EntityGenerator();
        ~EntityGenerator();

        std::optional<Entity> createEntity();
        void removeEntity(Entity entity);
        bool isAlive(Entity entity);

    private:
        size_t nextId;
        std::unordered_set<Entity> deadEntities;

};


#endif //JAREP_ENTITY_GENERATOR_HPP
