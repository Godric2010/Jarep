//
// Created by Sebastian Borsch on 01.07.23.
//

#ifndef JAREP_WORLD_HPP
#define JAREP_WORLD_HPP

#include <iostream>
#include <utility>
#include <vector>
#include <memory>
#include <unordered_map>
#include "entitymanager.hpp"
#include "componentmanager.hpp"
#include "systemmanager.hpp"

/// The world class is the top instance of the the JAREP-ECS. It manages the entity-, component- and system manager instances and
/// provides the necessary interfaces to interact with components and systems from outside the ecs.
class World {
	public:

		World() {
			entityManager = std::make_unique<EntityManager>();
			componentManager = std::make_unique<ComponentManager>();
			systemManager = std::make_unique<SystemManager>();
		}

		~World() = default;

		/// Create a new entity with no components attached.
		/// \return An empty entity instance if the creation was successful. Nullopt otherwise
		std::optional<Entity> createNewEntity() {
			auto newEntityResult = entityManager->createEntity();
			if (!newEntityResult.has_value()) return std::nullopt;

			auto newEntity = newEntityResult.value();
			const int archetypeIndex = 0;
			entityManager->assignNewSignature(newEntity, Signature(0), archetypeIndex);
			return std::make_optional(newEntity);
		}

		/// Remove an entity. All component instances will be destroyed in the process.
		/// \param entity The entity to destroy.
		void removeEntity(Entity entity) {

			auto entitySignature = entityManager->getSignature(entity);
			auto entityArchetypeIndex = entityManager->getArchetypeIndex(entity);

			if (!entitySignature.has_value() || !entityArchetypeIndex.has_value()) {
				return;
			}

			componentManager->removeEntityComponents(entitySignature.value(), entityArchetypeIndex.value());
			systemManager->removeEntityFromSystems(entity);

			entityManager->removeEntity(entity);

		}

		/// Add a component to an entity. The component will be initialized with default values and be linked to the entity passed in the parameter.
		/// Furthermore all system that require this component type will be linked to the entity argument.
		/// \tparam T The type of component to add. Must derive from Component.
		/// \param entity The entity this component shall be referenced to.
		template<class T, class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
		void addComponent(const Entity entity) {

			// Register the component if this is the first time it's been added to an entity.
			if (!componentManager->isComponentRegistred(typeid(T))) {
				componentManager->registerComponent<T>();
			}

			// Check if the provided entity is valid.
			auto oldSignature = entityManager->getSignature(entity);
			if (!oldSignature.has_value()) return;

			auto oldArchetypeIndex = entityManager->getArchetypeIndex(entity);
			if (!oldArchetypeIndex.has_value()) return;

			// Assign the component to the entity and retrieve the new signature and archetype index of the entity.
			// The Archetypes signature and the index at which the component instance is stored within the archetype are the
			// identifiers, each component instance is linked to a single entity by.
			auto newEntityData = componentManager->addComponentToSignature(oldSignature.value(),
			                                                               oldArchetypeIndex.value(),
			                                                               std::make_shared<T>());
			// Check if the component was assigned correctly.
			if (!newEntityData.has_value()) return;

			// Assign the new signature and archetype index to the entity. Now the component instance is linked to the
			// argument entity.
			auto newSignature = newEntityData.value().first;
			auto newArchetypeIndex = newEntityData.value().second;
			entityManager->assignNewSignature(entity, newSignature, newArchetypeIndex);

			// Collect all system which require the component type in their signature. The entity gets linked to these systems.
			auto newEntityAccessors = std::unordered_map<Entity, std::tuple<Signature, size_t>>();
			newEntityAccessors[entity] = std::make_tuple(newSignature, newArchetypeIndex);
			for (const auto &systemId: systemManager->getSystemsContainingSignature(oldSignature.value())) {
				systemManager->addEntitiesToSystem(systemId, newEntityAccessors);
			}
		}

		/// Remove an component from an entity. The instance of the component will be destroyed. Also the entity will be dereferenced from
		/// all systems that require the given component type.
		/// \tparam T The type of component to remove. Must derive of Component.
		/// \param entity The entity the given component type belongs to.
		template<class T, class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
		void removeComponent(Entity entity) {

			auto oldSignature = entityManager->getSignature(entity);
			auto oldArchetypeIndex = entityManager->getArchetypeIndex(entity);

			if (!oldSignature.has_value() || !oldArchetypeIndex.has_value()) {
				return;
			}

			auto newEntityData = componentManager->removeComponentFromSignature<T>(oldSignature.value(), oldArchetypeIndex.value());
			if (!newEntityData.has_value()) return;

			Signature newSignature = newEntityData.value().first;
			size_t newArchetypeIndex = newEntityData.value().second;
			entityManager->assignNewSignature(entity, newSignature, newArchetypeIndex);

			systemManager->removeEntityFromSystem(entity, oldSignature.value());
		}

		/// Register a system for updates during the update cycle. A new instance of the system will be created and existing components and entities that are
		/// required will be linked in the process.
		/// \tparam T The type of system to register. Must derive of System.
		/// \param requiredComponents A collection of all component type indices that are required by the system.
		/// \return True if the registration was successful, false if an error occurred.
		template<class T, class = typename std::enable_if<std::is_base_of<System, T>::value>::type>
		bool registerSystem(std::vector<std::type_index> requiredComponents) {

			auto systemSignatureResult = componentManager->getCombinedSignatureOfTypes(std::move(requiredComponents));
			if (!systemSignatureResult.has_value()) throw std::exception();


			auto getComponentsFunc = std::make_shared<GetComponentsFunc>(this->componentManager);
			auto systemIndexResult = systemManager->registerSystem<T>(systemSignatureResult.value(), getComponentsFunc);

			if (!systemIndexResult.has_value()) return false;

			auto allEntities = entityManager->getAllActiveEntities();
			auto entitiesContainingSignature = getAllEntitiesThatHaveThisSignature(allEntities,
			                                                                       systemSignatureResult.value());

			systemManager->addEntitiesToSystem(systemIndexResult.value(), entitiesContainingSignature);
			return true;
		}

		/// Deregister a system from the update loop. The system will no longer be updated on tick.
		/// \tparam T The type of system to deregister. Must derive from System.
		template<class T, class = typename std::enable_if<std::is_base_of<System, T>::value>::type>
		void deregisterSystem() {
			systemManager->unregisterSystem<T>();

		}

		/// RecreateSurface all systems
		void tick() {
			systemManager->update();
		}


	private:
		std::unique_ptr<EntityManager> entityManager;
		std::shared_ptr<ComponentManager> componentManager;
		std::unique_ptr<SystemManager> systemManager;

		std::unordered_map<Entity, std::tuple<Signature, size_t>>
		getAllEntitiesThatHaveThisSignature(const std::vector<Entity> &entitiesToCheck, Signature requestedSignature) {

			std::unordered_map<Entity, std::tuple<Signature, size_t>> entitiesContainingSignature;
			for (const auto &entity: entitiesToCheck) {
				auto entitySignatureResult = entityManager->getSignature(entity);
				auto entityArchetypeIndexResult = entityManager->getArchetypeIndex(entity);
				if (!entitySignatureResult.has_value() || !entityArchetypeIndexResult.has_value()) continue;
				auto entitySignature = entitySignatureResult.value();
				auto entityArchetypeIndex = entityArchetypeIndexResult.value();

				if ((entitySignature & requestedSignature) != requestedSignature) continue;

				auto signatureIndexTuple = std::make_tuple(entitySignature, entityArchetypeIndex);
				entitiesContainingSignature.insert_or_assign(entity, signatureIndexTuple);
			}
			return entitiesContainingSignature;
		}

		friend class WorldFriendAccessor;
};


#endif //JAREP_WORLD_HPP
