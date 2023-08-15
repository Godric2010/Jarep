//
// Created by Sebastian Borsch on 01.07.23.
//

#ifndef JAREP_WORLD_HPP
#define JAREP_WORLD_HPP


#include <iostream>
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

			auto newSignature = componentManager->addComponentToSignature(entityManager->getSignature(entity), entityManager->getArchetypeIndex(entity), std::make_shared<T>());
			if(!newSignature.hasValue()) return;

			entityManager->assignNewSignature(newSignature.value().item1, newSignature.value().item2);


		}

		template<class T, class = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
		void removeComponent(Entity entity) {

		}

		template<class T, class = typename std::enable_if<std::is_base_of<System, T>::value>::type>
		void registerSystem(std::vector<std::type_index> requiredComponents) {
			systemManager->registerSystem<T>(requiredComponents);

		}

		template<class T, class = typename std::enable_if<std::is_base_of<System, T>::value>::type>
		void deregisterSystem() {

		}


	private:
		std::unique_ptr<EntityManager> entityManager;
		std::unique_ptr<ComponentManager> componentManager;
		std::unique_ptr<SystemManager> systemManager;

};


#endif //JAREP_WORLD_HPP
