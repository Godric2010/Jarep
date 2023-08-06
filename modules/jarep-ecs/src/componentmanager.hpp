//
// Created by sebastian on 04.08.23.
//

#ifndef JAREP_COMPONENTMANAGER_HPP
#define JAREP_COMPONENTMANAGER_HPP

#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <type_traits>
#include <tuple>
#include <vector>
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
        void registerComponent() {
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
        std::optional<std::pair<Signature, size_t>>
        addComponentToSignature(Signature oldSignature, size_t entityIndex, T component) {

            if (!archetypeSignatureMap.contains(oldSignature)) return std::nullopt;

            auto newSignature = oldSignature | componentBitMap[typeid(T)];

            std::optional<size_t> newEntityIndex;
            if (archetypeSignatureMap.contains(newSignature)) {
                newEntityIndex = archetypeSignatureMap[newSignature]->migrateEntity(archetypeSignatureMap[oldSignature],
                                                                                    entityIndex);
            } else {
                auto newArchetype = Archetype::createFromAdd<T>(archetypeSignatureMap[oldSignature]).value();
                newEntityIndex = newArchetype.migrateEntity(archetypeSignatureMap[oldSignature], entityIndex);
                archetypeSignatureMap.insert_or_assign(newSignature, std::move(newArchetype));
            }

            if (!newEntityIndex.has_value()) return std::nullopt;

            archetypeSignatureMap[newSignature]->setComponentInstance(component);
            return std::make_optional(std::make_pair(newSignature, newEntityIndex.value()));
        }

        /// Remove a component from a signature
        /// \tparam T The component type to remove. Must be a deriving class of component.
        /// \param oldSignature The old signature this component shall be removed from.
        /// \param entityIndex The index of the entity to which the signature change shall occur
        /// \return Optional pair of the new signature (item1) and the new entity index (item2)
        template<class T, class = class std::enable_if<std::is_base_of<Component, T>::value>::type>
        std::optional<std::pair<Signature, size_t>>
        removeComponentFromSignature(Signature oldSignature, size_t entityIndex) {

            if (!archetypeSignatureMap.contains(oldSignature)) return std::nullopt;

            auto newSignature = oldSignature & ~componentBitMap[typeid(T)];

            std::optional<size_t> newEntityIndex;
            if (archetypeSignatureMap.contains(newSignature)) {
                newEntityIndex = archetypeSignatureMap[newSignature]->migrateEntity(archetypeSignatureMap[oldSignature],
                                                                                    entityIndex);
            } else {
                auto newArchetype = Archetype::createFromRemove<T>(archetypeSignatureMap[oldSignature]).value();
                newEntityIndex = newArchetype.migrateEntity(archetypeSignatureMap[oldSignature], entityIndex);
                archetypeSignatureMap.insert_or_assign(newSignature, std::move(newArchetype));
            }

            if (!newEntityIndex.has_value()) return std::nullopt;

            return std::make_optional(std::make_pair(newSignature, newEntityIndex.value()));
        }


        /// Get the instance of a component
        /// \tparam T The type of the requested component.
        /// \param signature The signature of the archetype, this component is stored in.
        /// \param entityIndex The index of the entity in the archetype.
        /// \return Optional shared pointer to the component instance.
        template<class T, class = class std::enable_if<std::is_base_of<Component, T>::value>::type>
        std::optional<std::shared_ptr<T>> getComponent(Signature signature, size_t entityIndex) {
            return archetypeSignatureMap[signature]->getComponent<T>(entityIndex);
        }


        /// Collect all components of the requested types and return them with their respected signature and entity index for identification.
        /// \tparam T The requested component type
        /// \return Collection of all components of this type, alongside the signature of the archetype they are stored in and the entity index.
        template<class T, class = class std::enable_if<std::is_base_of<Component, T>::value>::type>
        std::optional<std::vector<std::tuple<std::shared_ptr<T>, Signature, size_t>>> getComponentsOfType() {
            auto componentSignature = componentBitMap[typeid(T)];

            auto results = std::vector<std::tuple<std::shared_ptr<T>, Signature, size_t >>();
            for(const auto& signatureArchetype: archetypeSignatureMap){
                if((componentSignature & signatureArchetype.first) != componentSignature) continue;
                std::vector<std::shared_ptr<T>> componentsInArchetype = signatureArchetype.second->getComponentsWithEntities<T>();
                for (int i = 0; i < componentsInArchetype.size(); ++i) {
                   results.push_back(std::make_tuple(componentsInArchetype[i], signatureArchetype.second, i));
                } 
            }

            return std::make_optional(results);
        }

        /// Remove an entity from an archetype without migrating it to another
        /// \param signature The signature, this entity refers to.
        /// \param entityIndex The index of the entity at which the components are stored in the archetype.
        void removeEntityComponents(Signature signature, size_t entityIndex){
            if(!archetypeSignatureMap.contains(signature)) return;
            archetypeSignatureMap[signature]->removeComponentsAtEntityIndex(entityIndex);
        }


    private:
        std::unordered_map<Signature, std::unique_ptr<Archetype>> archetypeSignatureMap;

        std::unordered_map<std::type_index, Signature> componentBitMap;
        std::size_t nextComponentType;
};


#endif //JAREP_COMPONENTMANAGER_HPP
