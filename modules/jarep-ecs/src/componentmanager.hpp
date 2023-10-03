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
#include <utility>
#include <vector>
#include <iostream>
#include "signature.hpp"
#include "component.hpp"
#include "archetype.hpp"

class ComponentManager {

	public:
		ComponentManager() {
			nextComponentType = 1;
			componentBitMap = std::unordered_map<std::type_index, Signature>();

			archetypeSignatureMap = std::unordered_map<Signature, std::unique_ptr<Archetype>>();
			archetypeSignatureMap.insert_or_assign(0, Archetype::createEmpty());
		}

		~ComponentManager() = default;

		/// Register a component for usage in the ecs System. Each component must be registered before the first usage.
		/// \tparam T The component type to register. Must be a deriving class of Component
		template<class T, class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
		void registerComponent() {
			if (nextComponentType >= MAX_COMPONENTS) {
				return;
			}

			if (componentBitMap.contains(std::type_index(typeid(T)))) return;

			componentBitMap.insert_or_assign(std::type_index(typeid(T)), Signature(nextComponentType));
			++nextComponentType;
		}

		/// Check if a component is already registered.
		/// \param typeIndex The type index to check for registration.
		/// \return True if the component is already registered.
		bool isComponentRegistred(std::type_index typeIndex) {
			return componentBitMap.contains(typeIndex);
		}

		/// Add a component to a signature
		/// \tparam T The component type to add. Must be a deriving class of Component
		/// \param oldSignature The old signature this component shall be added to
		/// \param entityIndex The index of the entity to which the signature change shall occur
		/// \param component The instance of the component to add
		/// \return Optional pair of the new signature (item1) and the new entity index (item2)
		template<class T, class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
		std::optional<std::pair<Signature, size_t>>
		addComponentToSignature(Signature oldSignature, size_t entityIndex, std::shared_ptr<T> component) {

			if (!archetypeSignatureMap.contains(oldSignature)) return std::nullopt;

			auto newSignature = oldSignature | componentBitMap[typeid(T)];
			std::optional<size_t> newEntityIndex;
			if (archetypeSignatureMap.contains(newSignature)) {
				newEntityIndex = archetypeSignatureMap[newSignature]->migrateEntity(archetypeSignatureMap[oldSignature],
				                                                                    entityIndex);
			} else {
				auto newArchetype = Archetype::createFromAdd<T>(archetypeSignatureMap[oldSignature]).value();
				newEntityIndex = newArchetype->migrateEntity(archetypeSignatureMap[oldSignature], entityIndex);
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
		template<class T, class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
		std::optional<std::pair<Signature, size_t>>
		removeComponentFromSignature(Signature oldSignature, size_t entityIndex) {

			if (!componentBitMap.contains(typeid(T)) || !archetypeSignatureMap.contains(oldSignature)) {
				return std::nullopt;
			}

			if ((oldSignature & componentBitMap[typeid(T)]) != 0) return std::nullopt;

			auto newSignature = oldSignature & ~componentBitMap[typeid(T)];
			if (newSignature == Signature(0)) {
				removeEntityComponents(oldSignature, entityIndex);
				return std::make_optional(std::make_pair(newSignature, 0));
			}

			std::optional<size_t> newEntityIndex;
			if (archetypeSignatureMap.contains(newSignature)) {
				newEntityIndex = archetypeSignatureMap[newSignature]->migrateEntity(archetypeSignatureMap[oldSignature],
				                                                                    entityIndex);
			} else {
				auto newArchetype = Archetype::createFromRemove<T>(archetypeSignatureMap[oldSignature]).value();
				newEntityIndex = newArchetype->migrateEntity(archetypeSignatureMap[oldSignature], entityIndex);
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
		template<class T, class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
		std::optional<std::shared_ptr<T>> getComponent(Signature signature, size_t entityIndex) {

			if (!archetypeSignatureMap.contains(signature)) {
				return std::nullopt;
			}

			return archetypeSignatureMap.at(signature)->getComponent<T>(entityIndex);
		}


		/// Get the instance of a component
		/// \param archetypeSignature The archetypeSignature of the archetype, this component is stored in.
		/// \param entityIndex The index of the entity in the archetype.
		/// \param requiredComponent The type index of the requested component.
		/// \return Optional shared pointer to the component instance.
		template<class T, class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
		std::optional<std::shared_ptr<T>>
		getComponent(Signature archetypeSignature, size_t entityIndex, std::type_index requiredComponent) {

			if (!archetypeSignatureMap.contains(archetypeSignature)) {
				return std::nullopt;
			}

			return archetypeSignatureMap.at(archetypeSignature)->getComponent<T>(entityIndex, requiredComponent);
		}


		/// Collect all components of the requested types and return them with their respected signature and entity index for identification.
		/// \tparam T The requested component type
		/// \return Collection of all components of this type, alongside the signature of the archetype they are stored in and the entity index.
		template<class T, class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
		std::optional<std::vector<std::tuple<std::shared_ptr<T>, Signature, size_t>>> getComponentsOfType() {
			auto componentSignature = componentBitMap[typeid(T)];

			auto results = std::vector<std::tuple<std::shared_ptr<T>, Signature, size_t >>();
			for (const auto &signatureArchetype: archetypeSignatureMap) {
				if ((componentSignature & signatureArchetype.first) != componentSignature) continue;
				std::vector<std::shared_ptr<T>> componentsInArchetype = signatureArchetype.second->getComponentsWithEntities<T>();
				for (int i = 0; i < componentsInArchetype.size(); ++i) {
					results.push_back(std::make_tuple(componentsInArchetype[i], signatureArchetype.first, i));
				}
			}

			return std::make_optional(results);
		}

		/// Remove an entity from an archetype without migrating it to another
		/// \param signature The signature, this entity refers to.
		/// \param entityIndex The index of the entity at which the components are stored in the archetype.
		void removeEntityComponents(Signature signature, size_t entityIndex) {
			if (!archetypeSignatureMap.contains(signature)) return;
			archetypeSignatureMap[signature]->removeComponentsAtEntityIndex(entityIndex);
		}


		std::optional<Signature> getCombinedSignatureOfTypes(std::vector<std::type_index> typeIndices) {
			Signature resultSignature;

			for (const auto &typeIndex: typeIndices) {

				auto signatureResult = getSignatureOfType(typeIndex);
				if (!signatureResult.has_value()) return std::nullopt;

				resultSignature |= signatureResult.value();
			}
			return std::make_optional(resultSignature);
		}


	private:
		std::unordered_map<Signature, std::unique_ptr<Archetype>> archetypeSignatureMap;

		std::unordered_map<std::type_index, Signature> componentBitMap;
		std::size_t nextComponentType;

		std::optional<Signature> getSignatureOfType(std::type_index typeIndex) {
			if (componentBitMap.contains(typeIndex)) {
				return std::make_optional(componentBitMap.at(typeIndex));
			}
			return std::nullopt;
		}

		friend class WorldFriendAccessor;
};

class GetComponentsFunc {

	private:
		std::shared_ptr<ComponentManager> componentManager;

	public:
		explicit GetComponentsFunc(std::shared_ptr<ComponentManager> cm) : componentManager(std::move(cm)) {}

		template<typename T>
		std::optional<std::shared_ptr<T>> operator()(Signature signature, size_t entityIndex) const {
			return componentManager->getComponent<T>(signature, entityIndex, typeid(T));
		}
};

#endif //JAREP_COMPONENTMANAGER_HPP
