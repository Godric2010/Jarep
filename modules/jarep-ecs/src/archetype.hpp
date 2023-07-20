//
// Created by Sebastian Borsch on 01.07.23.
//

#ifndef JAREP_ARCHETYPE_HPP
#define JAREP_ARCHETYPE_HPP

#include <vector>
#include <unordered_map>
#include <typeindex>
#include <tuple>
#include <optional>
#include "componentInstanceCollection.hpp"
#include "entitygenerator.hpp"

class Archetype {

    public:
        Archetype();
        ~Archetype();

        /// Create a basic archetype with no components at all.
        /// \return An instance of a the most basic archetype.
        static std::unique_ptr<Archetype> createEmpty();

        /// Create a new archetype, based on another archetype by adding a new component.
        /// \tparam T -> The new component type to add.
        /// \param fromArchetype -> The old archetype this one is based on.
        /// \return A new instance of an archetype.
        template<typename T>
        static std::optional<std::unique_ptr<Archetype>> createFromAdd(const std::unique_ptr<Archetype> &fromArchetype);

        /// Create a new archetype, based on another archetype by removing a component.
        /// \tparam T -> The component type to remove.
        /// \param fromArchetype -> The old archetype this one is based on.
        /// \return A new instance of an archetype.
        template<class T>
        static std::optional<std::unique_ptr<Archetype>> createFromRemove(const std::unique_ptr<Archetype> &fromArchetype);

        template<class T>
        bool containsType();

        void removeEntity(Entity &entity);

        template<class T>
        void setComponentInstance(T componentInstance);

        template<class T>
        std::optional<T *> getComponent(size_t index);

        template<class T>
        std::optional<std::vector<std::tuple<T *, Entity>>> getComponentsWithEntities();

        void migrateEntity(std::unique_ptr<Archetype> &from, const Entity& entity);

        std::vector<Entity> entities;
    private:


        void generate_hash(std::vector<std::type_index> *componentTypes);

        std::unordered_map<std::type_index, size_t> componentTypeMap;
        std::size_t typeHash{};
        std::size_t componentCollectionsLength{};
        std::vector<std::unique_ptr<ComponentInstanceCollection>> componentCollections;

};


#endif //JAREP_ARCHETYPE_HPP
