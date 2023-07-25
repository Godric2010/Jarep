//
// Created by Sebastian Borsch on 01.07.23.
//

#ifndef JAREP_ARCHETYPE_HPP
#define JAREP_ARCHETYPE_HPP

#include <vector>
#include <unordered_map>
#include <typeindex>
#include <tuple>
#include <memory>
#include <optional>
#include "componentInstanceCollection.hpp"
#include "entity-generator.hpp"
#include "Component.hpp"

/// The archetype contains all entites with their respected component instances.
/// The implementation of the generic functions has to happen in the header to tackle linker issues when
/// attaching generic types from outside the scope of the archetype files.
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
        static std::optional<std::unique_ptr<Archetype>> createFromAdd(const std::unique_ptr<Archetype> &fromArchetype){
            auto instance = std::make_unique<Archetype>();

            // Take the existing archetype and create a new component instance collection with only empty vectors.
            size_t fromColumnLength = fromArchetype->componentCollections.size();
            for (int i = 0; i < fromColumnLength; ++i) {
                auto newAndEmpty = fromArchetype->componentCollections[i]->createNewAndEmpty();
                instance->componentCollections.push_back(std::move(newAndEmpty));
            }
            std::function<std::unique_ptr<ComponentInstanceCollection>()> createInstanceCollection =[](){
                return std::make_unique<InstanceCollection<T>>();
            };
            std::unique_ptr<ComponentInstanceCollection> typeErasedCollection = createInstanceCollection();
            instance->componentCollections.push_back(std::move(typeErasedCollection));
            instance->componentCollectionsLength += 1;

            // Iterate over the existing type-map and copy it, as well as collecting all types from the old archetype.
            instance->componentTypeMap = std::unordered_map<std::type_index, size_t>();
            auto typesInArchetype = new std::vector<std::type_index>();
            for (const auto typeEntry: fromArchetype->componentTypeMap) {
                instance->componentTypeMap.insert_or_assign(typeEntry.first, typeEntry.second);
                typesInArchetype->push_back(typeEntry.first);
            }

            // Assign the new generic component to all maps and lists so the new archetype will be different from the old one.
            instance->componentTypeMap.insert_or_assign(typeid(T), instance->componentCollectionsLength);
            typesInArchetype->push_back(typeid(T));

            instance->generate_hash(typesInArchetype);

            return std::make_optional<std::unique_ptr<Archetype>>(std::move(instance));
        };

        /// Create a new archetype, based on another archetype by removing a component.
        /// \tparam T -> The component type to remove.
        /// \param fromArchetype -> The old archetype this one is based on.
        /// \return A new instance of an archetype.
        template<class T>
        static std::optional<std::unique_ptr<Archetype>> createFromRemove(const std::unique_ptr<Archetype> &fromArchetype){
            // Check if the old archetype contains the requested type to remove and return nullopt if not.
            if (!fromArchetype->containsType<T>()) {
                return std::nullopt;
            }

            auto instance = new Archetype();

            // Start with coping the existing types and filter out the index of the type to remove in the process.
            instance->componentTypeMap = std::unordered_map<std::type_index, size_t>();
            auto typesInArchetype = new std::vector<std::type_index>();
            size_t targetIndexToRemove = 0;
            for (const auto typeEntry: fromArchetype->componentTypeMap) {

                // If the typeid is the one to remove we do not copy it memorize the index of that type in the archetype lists.
                if (typeEntry.first == typeid(T)) {
                    targetIndexToRemove = typeEntry.second;
                    continue;
                }

                instance->componentTypeMap.insert_or_assign(typeEntry.first, typeEntry.second);
                typesInArchetype->push_back(typeEntry.first);
            }
            instance->generate_hash(typesInArchetype);

            // Copy the component lists and instantiate them empty except for the collection at the memorized index.
            size_t newComponentCollectionsLength = fromArchetype->componentCollectionsLength--;
            for (int i = 0; i < newComponentCollectionsLength; ++i) {
                if (i == targetIndexToRemove) continue;
                instance->componentCollections[i] = fromArchetype->componentCollections[i]->createNewAndEmpty();
            }

            return std::make_optional<std::unique_ptr<Archetype>>(instance);
        }

        template<class T>
        bool containsType(){
            auto result = componentTypeMap.find(typeid(T));
            if (componentTypeMap.end() != result) {
                return false;
            }
            return true;
        }

        void removeEntity(Entity &entity);

        template<class T>
        void setComponentInstance(T componentInstance){

        }

        template<class T>
        std::optional<T *> getComponent(size_t index){

        }

        template<class T>
        std::optional<std::vector<std::tuple<T *, Entity>>> getComponentsWithEntities(){

        }

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
