//
// Created by Sebastian Borsch on 01.07.23.
//

#ifndef JAREP_WORLD_HPP
#define JAREP_WORLD_HPP


#include <iostream>
#include <vector>
#include <unordered_map>
#include "entitymanager.hpp"
#include "archetype.hpp"


class World {
    public:
        World() {
            entityGenerator = new EntityManager();
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

        template<class T>
        void addComponent(Entity entity, std::shared_ptr<T> component) {

        }

        template<class T>
        void removeComponent(Entity entity) {

        }

        template<class T>
        void registerSystem() {

        }

        template<class T>
        void deregisterSystem() {

        }


    private:
        EntityManager *entityGenerator;
        std::unordered_map<Entity, int> entityLocationMap;
        std::vector<Archetype> archetypes;

};


#endif //JAREP_WORLD_HPP
