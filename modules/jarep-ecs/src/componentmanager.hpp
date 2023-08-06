//
// Created by sebastian on 04.08.23.
//

#ifndef JAREP_COMPONENTMANAGER_HPP
#define JAREP_COMPONENTMANAGER_HPP

#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <type_traits>
#include "signature.hpp"
#include "Component.hpp"
#include "archetype.hpp"

class ComponentManager {

    public:
        ComponentManager() {
            nextComponentType = 0;
            componentBitMap = std::unordered_map<std::type_index, Signature>();

            archetypeSignatureMap = std::unordered_map<Signature, std::unique_ptr<Archetype>>();
            archetypeSignatureMap.insert_or_assign(0, Archetype::createEmpty());
        }

        ~ComponentManager() = default;

        /// Register a component for usage in the ecs system. Each component must be registered before the first usage.
        /// \tparam T The component type to register. Must be a deriving class of Component
        template<class T, class = class std::enable_if<std::is_base_of<Component, T>::value>::type>
        void regsisterComponent() {
            if (nextComponentType >= MAX_COMPONENTS) {
                return;
            }
            componentBitMap[std::type_index(typeid(T))] = Signature(nextComponentType);
            ++nextComponentType;
        }

        /// Add a component to a signature
        /// \tparam T The component type to add. Must be a deriving class of Component
        /// \param oldSignature The old signature this component shall be added to
        /// \param entityIndex The index of the entity to which the signature change shall occur
        /// \param component The instance of the component to add
        /// \return Optional pair of the new signature (item1) and the new entity index (item2)
        template<class T, class = class std::enable_if<std::is_base_of<Component, T>::value>::type>
        std::optional<std::pair<Signature,size_t>> addComponentToSignature(Signature oldSignature, size_t entityIndex, T component) {

            if (!archetypeSignatureMap.contains(oldSignature)) return std::nullopt;

            auto newSignature = oldSignature | componentBitMap[typeid(T)];

            std::optional<size_t> newEntityIndex;
            if (archetypeSignatureMap.contains(newSignature)) {
               newEntityIndex = archetypeSignatureMap[newSignature]->migrateEntity(archetypeSignatureMap[oldSignature], entityIndex);
            } else {
                auto newArchetype = Archetype::createFromAdd<T>(archetypeSignatureMap[oldSignature]);
                newEntityIndex = newArchetype.migrateEntity(archetypeSignatureMap[oldSignature], entityIndex);
                archetypeSignatureMap.insert_or_assign(newSignature, std::move(newArchetype));
            }

            if(!newEntityIndex.has_value()) return std::nullopt;

            archetypeSignatureMap[newSignature]->setComponentInstance(component);
            return std::make_optional(std::make_pair(newSignature, newEntityIndex.value()));
        }

        /// Remove a component from a signature
        /// \tparam T The component type to remove. Must be a deriving class of component.
        /// \param oldSignature The old signature this component shall be removed from.
        /// \param enityIndex The index of the entity to which the signature change shall occur
        /// \return Optional pair of the new signature (item1) and the new entity index (item2)
        template<class T, class = class std::enable_if<std::is_base_of<Component, T>::value>::type>
        std::optional<std::pair<Signature, size_t>> removeComponentFromSignature(Signature oldSignature, size_t enityIndex) {

            if(!archetypeSignatureMap.contains(oldSignature)) return std::nullopt;

            auto newSignature = oldSignature & ~componentBitMap[typeid(T)];

            std::optional<size_t> newEntityIndex;
            if(archetypeSignatureMap.contains(newSignature)){
                newEntityIndex = archetypeSignatureMap[newSignature]->migrateEntity(archetypeSignatureMap[oldSignature], enityIndex);
            } else{
                auto newArchetype = Archetype::createFromRemove<T>(archetypeSignatureMap[oldSignature]);
                newEntityIndex = newArchetype.migrateEntity(archetypeSignatureMap[oldSignature], enityIndex);
                archetypeSignatureMap.insert_or_assign(newSignature, std::move(newArchetype));
            }

            if(!newEntityIndex.has_value()) return std::nullopt;

            return std::make_optional(std::make_pair(newSignature, newEntityIndex.value()));
        }





    private:
        std::unordered_map<Signature, std::unique_ptr<Archetype>> archetypeSignatureMap;

        std::unordered_map<std::type_index, Signature> componentBitMap;
        std::size_t nextComponentType;
};


#endif //JAREP_COMPONENTMANAGER_HPP
