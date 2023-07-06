//
// Created by Sebastian Borsch on 01.07.23.
//

#ifndef JAREP_ARCHETYPE_HPP
#define JAREP_ARCHETYPE_HPP

#include <vector>
#include <unordered_map>
#include <typeindex>
#include "componentInstanceCollection.hpp"
#include "entitygenerator.hpp"

class Archetype {

    public:
        ~Archetype() = default;

        static Archetype* createEmpty();

        template<class T>
        static Archetype* createFromAdd(Archetype &fromArchetype);

        template<class T>
        static Archetype* createFromRemove(Archetype &fromArchetype);


    private:
        Archetype(){
            typeHash = 0;
        };


        std::vector<Entity> entities;
        std::unordered_map<std::type_index, size_t> componentTypeMap;
        std::vector<ComponentInstanceCollection> componentCollections;
        std::size_t typeHash;

        size_t generate_hash(std::vector<std::type_index> *componentTypes);

};




#endif //JAREP_ARCHETYPE_HPP
