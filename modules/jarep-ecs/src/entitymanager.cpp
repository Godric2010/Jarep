//
// Created by Sebastian Borsch on 01.07.23.
//

#include "entitymanager.hpp"

EntityManager::EntityManager() {
	nextId = 0;
	deadEntities = std::queue<Entity>();
	entitySignatureMap.clear();

}

EntityManager::~EntityManager() {
	nextId = 0;
	std::queue<Entity> empty;
	std::swap(deadEntities, empty);
	entitySignatureMap.clear();
}

std::optional<Entity> EntityManager::createEntity() {

	Entity newEntity;
	if (!deadEntities.empty()) {
		newEntity = deadEntities.front();
		deadEntities.pop();
	} else {
		newEntity = nextId;
		if (nextId == SIZE_MAX) {
			printf("Exceeded the maximum entities!");
			return std::nullopt;
		}
		nextId += 1;
	}

	return std::make_optional(newEntity);
}

void EntityManager::removeEntity(Entity entity) {

	if (!isAlive(entity)) return;
	deadEntities.push(entity);
	entitySignatureMap.erase(entity);
}

bool EntityManager::isAlive(Entity entity) const{
	if (entity >= nextId) {
		throw std::runtime_error("Requesting alive status for uninitialized entities is forbidden!");
	}

	auto deadEntitiesCopy = deadEntities;
	for (int i = 0; i < deadEntities.size(); ++i) {
		if(deadEntitiesCopy.front() == entity)
			return false;
		deadEntitiesCopy.pop();
	}
	return true;
}

void EntityManager::assignNewSignature(const Entity entity, const Signature signature, const size_t archetypeIndex) {
	entitySignatureMap[entity] = signature;
	entityArchetypeIndexMap[entity] = archetypeIndex;
}

std::optional<Signature> EntityManager::getSignature(const Entity entity) const {
	if(!isAlive(entity)) return std::nullopt;
	if(!entitySignatureMap.contains(entity)) return std::nullopt;
	return entitySignatureMap.at(entity);
}

std::optional<size_t> EntityManager::getArchetypeIndex(const Entity entity)const {
	if(!isAlive(entity)) return std::nullopt;
	if(!entityArchetypeIndexMap.contains(entity)) return std::nullopt;
	return entityArchetypeIndexMap.at(entity);
}
