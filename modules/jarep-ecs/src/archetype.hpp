//
// Created by Sebastian Borsch on 01.07.23.
//

#ifndef JAREP_ARCHETYPE_HPP
#define JAREP_ARCHETYPE_HPP

#include <vector>
#include <unordered_map>
#include <typeindex>
#include <tuple>
#include "componentInstanceCollection.hpp"
#include "entitygenerator.hpp"

class Archetype {

    public:
        ~Archetype() = default;

        static Archetype *createEmpty();

        template<class T>
        static Archetype *createFromAdd(Archetype &fromArchetype);

        template<class T>
        static Archetype *createFromRemove(Archetype &fromArchetype);

        template<class T>
        bool containsType();

        void removeEntity(Entity &entity);

        template<class T>
        void setComponentInstance(T componentInstance);

        template<class T>
        std::optional<T *> getComponent(size_t index);

        template<class T>
        std::optional<std::vector<std::tuple<T *, Entity>>> getComponentsWithEntities();

        void migrateEntity(Archetype *from, Entity entity);



        //template<class T>
        //std::optional<std::

    private:
        Archetype() {
            typeHash = 0;
            componentCollectionsLength = 0;
        };

        void generate_hash(std::vector<std::type_index> *componentTypes);

        std::vector<Entity> entities;
        std::unordered_map<std::type_index, size_t> componentTypeMap;
        std::size_t typeHash;
        std::size_t componentCollectionsLength;
        ComponentInstanceCollection* componentCollections[];

};


#endif //JAREP_ARCHETYPE_HPP
