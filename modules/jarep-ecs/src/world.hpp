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
			if(!newEntityResult.has_value()) return std::nullopt;

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

			if(!entitySignature.has_value() || !entityArchetypeIndex.has_value())
				return;

			componentManager->removeEntityComponents(entitySignature.value(), entityArchetypeIndex.value());
			systemManager->removeEntityFromSystems(entity);

			entityManager->removeEntity(entity);

		}

		template<class T, class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
		void addComponent(const Entity entity) {

			if (!componentManager->isComponentRegistred(typeid(T))) {
				componentManager->registerComponent<T>();
			}

			auto entity_signature = entityManager->getSignature(entity);
			if (!entity_signature.has_value()) return;

			auto entity_index_in_archetype = entityManager->getArchetypeIndex(entity);
			if (!entity_index_in_archetype.has_value()) return;

			auto newSignature = componentManager->addComponentToSignature(entity_signature.value(),
			                                                              entity_index_in_archetype.value(),
			                                                              std::make_shared<T>());
			if (!newSignature.has_value()) return;

			auto newSignatureValue = newSignature.value().first;
			auto newArchetypeIndexValue = newSignature.value().second;
			entityManager->assignNewSignature(entity, newSignatureValue, newArchetypeIndexValue);

			auto newEntityAccessors = std::unordered_map<Entity, std::tuple<Signature, size_t>>();
			newEntityAccessors[entity] = std::make_tuple(newSignatureValue, newArchetypeIndexValue);
			for(const auto& systemId : systemManager->getSystemsContainingSignature(entity_signature.value())){
				systemManager->setSystemData(systemId,  newEntityAccessors);
			}
		}

		template<class T, class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
		void removeComponent(Entity entity) {
			// TODO: Update archetype
			// TODO: update entity manager
			// TODO: update systems.
		}

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

			systemManager->setSystemData(systemIndexResult.value(), entitiesContainingSignature);
			return true;
		}

		template<class T, class = typename std::enable_if<std::is_base_of<System, T>::value>::type>
		void deregisterSystem() {

		}

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
