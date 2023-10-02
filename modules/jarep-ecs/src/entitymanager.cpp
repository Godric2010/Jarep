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
		if (nextId == std::numeric_limits<unsigned int>::max()) {
			printf("Exceeded the maximum entities!");
			return std::nullopt;
		}
		nextId += 1;
	}

	return std::make_optional(newEntity);
}

void EntityManager::removeEntity(Entity entity) {

	if (!isAlive(entity)) return;

	// Collect the signature and removed entity index of the entity that shall be removed for later usage.
	Signature entitySignature = entitySignatureMap[entity];
	size_t removedEntityIndex= entityArchetypeIndexMap[entity];

	// Remove the entity from all lists and mark the entity as dead. Now the entity does not exist anymore.
	deadEntities.push(entity);
	entitySignatureMap.erase(entity);
	entityArchetypeIndexMap.erase(entity);

	// Collect all entities that are also assigned to this signature and therefore the same archetype.
	auto entitiesWithSameSignature = getAllEntitiesOfSignature(entitySignature);

	// Since the entities are stored in a list inside the archetype, removing one causes their indices inside the list to decrease by one.
	// Therefore, we need to check which entities had a higher index in the archetype than the removed on and decrease their index by one.
	// If this is not done, the entities will refere to the wrong components or even cause an out-of-bounds exception.
	for (const Entity sameSignature: entitiesWithSameSignature) {
		if(entityArchetypeIndexMap[sameSignature] > removedEntityIndex)
			entityArchetypeIndexMap[sameSignature]--;
	}
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

std::vector<Entity> EntityManager::getAllEntitiesOfSignature(Signature signature)const {
	auto entitiesWithSignature = std::vector<Entity>();
	for(const auto& entitySignaturePair: entitySignatureMap){
		if(entitySignaturePair.second == signature)
			entitiesWithSignature.push_back(entitySignaturePair.first);
	}
	return entitiesWithSignature;
}