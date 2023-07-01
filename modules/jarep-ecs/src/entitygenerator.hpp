//
// Created by Sebastian Borsch on 01.07.23.
//

#ifndef JAREP_ENTITYGENERATOR_HPP
#define JAREP_ENTITYGENERATOR_HPP

#include <iostream>
#include <unordered_set>

typedef size_t Entity;

class EntityGenerator {
    public:
        EntityGenerator();
        ~EntityGenerator();

        Entity createEntity();
        void removeEntity(Entity entity);
        bool isAlive(Entity entity);

    private:
        size_t nextId;
        std::unordered_set<Entity> deadEntities;

};


#endif //JAREP_ENTITYGENERATOR_HPP
