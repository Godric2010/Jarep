//
// Created by Sebastian Borsch on 01.07.23.
//

#ifndef JAREP_ARCHETYPE_HPP
#define JAREP_ARCHETYPE_HPP

#include <vector>
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <tuple>
#include <memory>
#include <optional>
#include "componentInstanceCollection.hpp"
#include "entity-generator.hpp"
#include "Component.hpp"

/// The archetype contains all entities with their respected component instances.
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
        static std::optional<std::unique_ptr<Archetype>>
        createFromAdd(const std::unique_ptr<Archetype> &fromArchetype) {
            auto instance = std::make_unique<Archetype>();

            // Take the existing archetype and create a new component instance collection with only empty vectors.
            size_t fromColumnLength = fromArchetype->componentCollections.size();
            for (int i = 0; i < fromColumnLength; ++i) {
                auto newAndEmpty = fromArchetype->componentCollections[i]->createNewAndEmpty();
                instance->componentCollections.push_back(std::move(newAndEmpty));
            }
            std::function<std::unique_ptr<ComponentInstanceCollection>()> createInstanceCollection = []() {
                return std::make_unique<InstanceCollection<T>>();
            };
            std::unique_ptr<ComponentInstanceCollection> typeErasedCollection = createInstanceCollection();
            size_t newComponentIndex = instance->componentCollections.size();
            instance->componentCollections.push_back(std::move(typeErasedCollection));

            // Iterate over the existing type-map and copy it, as well as collecting all types from the old archetype.
            instance->componentTypeMap = std::unordered_map<std::type_index, size_t>();
            auto typesInArchetype = std::vector<std::type_index>();
            for (const auto typeEntry: fromArchetype->componentTypeMap) {
                instance->componentTypeMap.insert_or_assign(typeEntry.first, typeEntry.second);
                typesInArchetype.push_back(typeEntry.first);
            }

            // Assign the new generic component to all maps and lists so the new archetype will be different from the old one.
            instance->componentTypeMap.insert_or_assign(typeid(T), newComponentIndex);
            typesInArchetype.push_back(typeid(T));

            return std::make_optional<std::unique_ptr<Archetype>>(std::move(instance));
        };

        /// Create a new archetype, based on another archetype by removing a component.
        /// \tparam T -> The component type to remove.
        /// \param fromArchetype -> The old archetype this one is based on.
        /// \return A new instance of an archetype.
        template<class T>
        static std::optional<std::unique_ptr<Archetype>>
        createFromRemove(const std::unique_ptr<Archetype> &fromArchetype) {
            // Check if the old archetype contains the requested type to remove and return nullopt if not.
            if (!fromArchetype->containsType<T>()) {
                return std::nullopt;
            }

            auto instance = std::make_unique<Archetype>();

            // Start with coping the existing types and filter out the index of the type to remove in the process.
            instance->componentTypeMap = std::unordered_map<std::type_index, size_t>();
            auto typesInArchetype = std::vector<std::type_index>();
            size_t targetIndexToRemove = 0;
            for (const auto typeEntry: fromArchetype->componentTypeMap) {

                // If the typeid is the one to remove we do not copy it memorize the index of that type in the archetype lists.
                if (typeEntry.first == typeid(T)) {
                    targetIndexToRemove = typeEntry.second;
                    continue;
                }

                instance->componentTypeMap.insert_or_assign(typeEntry.first, typeEntry.second);
                typesInArchetype.push_back(typeEntry.first);
            }
            // Copy the component lists and instantiate them empty except for the collection at the memorized index.
            size_t newComponentCollectionsLength = fromArchetype->componentCollections.size() - 1;
            for (int i = 0; i < newComponentCollectionsLength; ++i) {
                if (i == targetIndexToRemove) continue;
                instance->componentCollections[i] = fromArchetype->componentCollections[i]->createNewAndEmpty();
            }

            return std::make_optional<std::unique_ptr<Archetype>>(std::move(instance));
        }

        /// Checks if an archetype is equipped with the requested component.
        /// \tparam T -> The component to test for.
        /// \return True if the archetype contains the component T, otherwise returns false.
        template<class T>
        bool containsType() {
            const std::type_info &typeId = typeid(T);
            if (componentTypeMap.count(typeId)) {
                return true;
            }
            return false;
        }

        /// Remove an entity from an archetype, including all the connected component instances to this entity
        /// \param entityIndex -> The entity index to remove.
        void removeComponentsAtEntityIndex(size_t entityIndex);

        /// Set an instance to a component
        /// \tparam T -> The type of the component instance to add
        /// \param componentInstance -> The component instance to add
        template<class T>
        void setComponentInstance(std::shared_ptr<T> componentInstance) {

            size_t component_index = componentTypeMap.at(typeid(T));
            auto &componentCollection = componentCollections.at(component_index);
            auto &target_collection = std::any_cast<std::reference_wrapper<std::vector<std::shared_ptr<T>>>>(
                    componentCollection->as_any()).get();
            target_collection.push_back(componentInstance);
        }

        /// Get the instance of a component by the index of the entity in this archetype.
        /// \tparam T -> The type of the component
        /// \param index -> The index of the entity in this component
        /// \return Pointer to the components instance
        template<class T>
        std::optional<std::shared_ptr<T>> getComponent(size_t index) {

            size_t component_index = componentTypeMap.at(typeid(T));
            auto &componentCollection = componentCollections.at(component_index);
            auto &target_collection = std::any_cast<std::reference_wrapper<std::vector<std::shared_ptr<T>>>>(
                    componentCollection->as_any()).get();
            if (target_collection.size() <= index) {
                return std::nullopt;
            }
            return std::make_optional(target_collection.at(index));
        }

        /// Get all instances of a specific component type and their respected entites.
        /// \tparam T -> The type of component to receive
        /// \return A list of tuples containing the pointer to the component instance and the respected entity.
        template<class T>
        std::vector<std::shared_ptr<T>> getComponentsWithEntities() {

            size_t component_index = componentTypeMap.at(typeid(T));
            auto &componentCollection = componentCollections.at(component_index);
            auto &target_collection = std::any_cast<std::reference_wrapper<std::vector<std::shared_ptr<T>>>>(
                    componentCollection->as_any()).get();
            return target_collection;
        }

        /// Migrate an entity with all of its components from one archetype to another one.
        /// \param from -> The "old" archetype, the entity shall migrate from
        /// \param entityIndex -> The entity index of the old archetype that shall be migrated.
        std::optional<size_t> migrateEntity(std::unique_ptr<Archetype> &from, const size_t &entityIndex);

    private:

        std::unordered_map<std::type_index, size_t> componentTypeMap;
        std::vector<std::unique_ptr<ComponentInstanceCollection>> componentCollections;

};

#endif //JAREP_ARCHETYPE_HPP
