//
// Created by Sebastian Borsch on 01.07.23.
//

#ifndef JAREP_WORLD_HPP
#define JAREP_WORLD_HPP


#include <iostream>
#include <vector>
#include <unordered_map>
#include "entity-generator.hpp"
#include "archetype.hpp"


class World {
    public:
        World() {
            entityGenerator = new EntityGenerator();
            entityLocationMap.clear();
            archetypes.clear();

        }

        ~World() {
            entityGenerator = nullptr;
            entityLocationMap.clear();
            archetypes.clear();
        }

        Entity createNewEntity();
        void removeEntity();



    private:
        EntityGenerator *entityGenerator;
        std::unordered_map<Entity, int> entityLocationMap;
        std::vector<Archetype> archetypes;

};


#endif //JAREP_WORLD_HPP
