//
// Created by Sebastian Borsch on 01.07.23.
//

#ifndef JAREP_ENTITYMANAGER_HPP
#define JAREP_ENTITYMANAGER_HPP

#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include "signature.hpp"

typedef size_t Entity;

class EntityManager {
    public:
        EntityManager();
        ~EntityManager();

		/// Create a new entity index
		/// \return A new entity index if the creation was successful. Nullopt otherwise.
        std::optional<Entity> createEntity();

		/// Remove an entity
		/// \param entity The entity to remove.
        void removeEntity(Entity entity);

		/// Check if an entity is still alive.
		/// \param entity The entity to check
		/// \return True if the entity is alive, false otherwise.
        bool isAlive(Entity entity);

		/// Assign a new signature to an entity
		/// \param entity The entity the new signature shall be assigned to.
		/// \param signature The new signature that shall be assigned to the entity.
		void assignNewSignature(Entity entity, Signature signature);

		std::optional<Signature> getSignature(Entity entity);

    private:
        size_t nextId;
        std::queue<Entity> deadEntities;
		std::unordered_map<Entity, Signature> entitySignatureMap;
};


#endif //JAREP_ENTITYMANAGER_HPP
