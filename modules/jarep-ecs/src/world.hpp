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

class World {
	public:
		World() {
			entityManager = std::make_unique<EntityManager>();
			componentManager = std::make_unique<ComponentManager>();
			systemManager = std::make_unique<SystemManager>();
		}

		~World() = default;

		Entity createNewEntity() {
			return entityManager->createEntity().value();
		}

		void removeEntity(Entity entity) {
			entityManager->removeEntity(entity);

		}

		template<class T, class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
		void addComponent(const Entity entity) {

			if(!componentManager->isComponentRegistred(typeid(T))){
				componentManager->registerComponent<T>();
			}

			auto entity_signature = entityManager->getSignature(entity);
			if(!entity_signature.has_value()) return;

			auto entity_index_in_archetype = entityManager->getArchetypeIndex(entity);
			if(!entity_index_in_archetype.has_value()) return;

			auto newSignature = componentManager->addComponentToSignature(entity_signature.value(), entity_index_in_archetype.value(), std::make_shared<T>());
			if(!newSignature.has_value()) return;

			entityManager->assignNewSignature(entity, newSignature.value().first, newSignature.value().second);

		}

		template<class T, class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
		void removeComponent(Entity entity) {

		}

		template<class T, class = typename std::enable_if<std::is_base_of<System, T>::value>::type>
		void registerSystem(std::vector<std::type_index> requiredComponents) {

			auto systemSignatureResult = componentManager->getCombinedSignatureOfTypes(std::move(requiredComponents));
			if(!systemSignatureResult.has_value()) return;


			auto getComponentsFunc = std::make_shared<GetComponentsFunc>(this->componentManager);
			auto systemIndexResult = systemManager->registerSystem<T>(systemSignatureResult.value(), getComponentsFunc);

			if(!systemIndexResult.has_value()) return;

			auto allEntities = entityManager->getAllActiveEntities();
			auto entitiesContainingSignature = getAllEntitiesThatHaveThisSignature(allEntities, systemSignatureResult.value());

			systemManager->updateSystemData(systemIndexResult.value(), entitiesContainingSignature);

		}

		template<class T, class = typename std::enable_if<std::is_base_of<System, T>::value>::type>
		void deregisterSystem() {

		}

		void tick(){
			systemManager->update();
		}


	private:
		std::unique_ptr<EntityManager> entityManager;
		std::shared_ptr<ComponentManager> componentManager;
		std::unique_ptr<SystemManager> systemManager;

		std::unordered_map<Entity, std::tuple<Signature, size_t>> getAllEntitiesThatHaveThisSignature(const std::vector<Entity>& entitiesToCheck, Signature requestedSignature){

			std::unordered_map<Entity, std::tuple<Signature, size_t>> entitiesContainingSignature;
			for (const auto &entity: entitiesToCheck) {
				auto entitySignatureResult = entityManager->getSignature(entity);
				auto entityArchetypeIndexResult = entityManager->getArchetypeIndex(entity);
				if(!entitySignatureResult.has_value() || !entityArchetypeIndexResult.has_value()) continue;
				auto entitySignature = entitySignatureResult.value();
				auto entityArchetypeIndex = entityArchetypeIndexResult.value();

				if((entitySignature & requestedSignature) != requestedSignature) continue;

				auto signatureIndexTuple = std::make_tuple(entitySignature, entityArchetypeIndex);
				entitiesContainingSignature.insert_or_assign(entity, signatureIndexTuple);
			}
			return entitiesContainingSignature;
		}


};


#endif //JAREP_WORLD_HPP
