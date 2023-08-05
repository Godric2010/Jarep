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

        template<class T, class = class std::enable_if<std::is_base_of<Component, T>::value>::type>
        void regsisterComponent() {
            if (nextComponentType >= MAX_COMPONENTS) {
                return;
            }
            componentBitMap[std::type_index(typeid(T))] = Signature(nextComponentType);
            ++nextComponentType;
        }

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
