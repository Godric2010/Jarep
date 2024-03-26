//
// Created by Sebastian Borsch on 10.08.23.
//

#ifndef JAREP_SYSTEMMANAGER_HPP
#define JAREP_SYSTEMMANAGER_HPP

#include <utility>
#include <vector>
#include <optional>
#include <typeindex>
#include "signature.hpp"
#include "componentmanager.hpp"
#include "system.hpp"

/// The system manager is responsible for dealing with all issues regarding the updating and maintaining of the system deriving classes.
class SystemManager {

	public:
		SystemManager() = default;
		~SystemManager() = default;

		/// Register a system for the update process
		/// \tparam T The type of the system that shall be registered. Must derive vom System
		/// \param systemSignature The Signature of the system, composed by the component signatures needed by this system.
		/// \param getComponentsFunc Functor to the component manager to access component data fast and easy.
		/// \return Optional type index of the system for further usage.
		template<class T, class = typename std::enable_if<std::is_base_of<System, T>::value>::type>
		std::optional<std::type_index> registerSystem(Signature systemSignature, std::shared_ptr<GetComponentsFunc> getComponentsFunc) {

			// If the system is already registered, another registration is illegal.
			if (isSystemRegistred(typeid(T))) return std::nullopt;

			// Create the system instance and prepare it.
			std::unique_ptr<System> system = std::make_unique<T>();
			system->getComponentFunc = std::move(getComponentsFunc);

			systemTypeIndexMap.insert_or_assign(typeid(T), std::move(system));
			systemSignatureMap.insert_or_assign(typeid(T), systemSignature);
			return std::make_optional(std::type_index(typeid(T)));
		}

		/// Unregister a system from the manager. It will excluded from being updated in all future update calls until it gets registered again.
		/// \tparam T The type of system to unregister.
		template<class T, class = typename std::enable_if<std::is_base_of<System, T>::value>::type>
		void unregisterSystem() {

			if (!systemTypeIndexMap.contains(typeid(T))) return;

			auto entities = systemTypeIndexMap[typeid(T)]->getEntities();

			systemTypeIndexMap.erase(typeid(T));
			systemSignatureMap.erase(typeid(T));

			for(const auto entity: entities){
				for(auto systemType: assignedEntitySystemMap[entity]){
					if(systemType == typeid(T)){
						auto vec = assignedEntitySystemMap[entity];
						vec.erase(std::remove(vec.begin(), vec.end(), systemType), vec.end());
						assignedEntitySystemMap[entity] = vec;
					}
				}
			}


		}

		/// RecreateSurface all systems registered in this manager.
		void update() {
			for (auto &system: systemTypeIndexMap) {
				system.second->update();
			}
		}

		/// RecreateSurface a system with new associated entities, signatures and their respected indices in the archetypes.
		/// \param systemType The type of the system to update. Since all systems can only be registered once, this identifier is unique for one system.
		/// \param entitiesWithAccessIds A map containing the entities with their respected Signature and archetype index available for this system.
		void addEntitiesToSystem(std::type_index systemType,
		                         const std::unordered_map<Entity, std::tuple<Signature, size_t>>& entitiesWithAccessIds) {

			if (!systemTypeIndexMap.contains(systemType)) return;

			for(auto& value_pair : entitiesWithAccessIds){
				assignedEntitySystemMap[value_pair.first].push_back(systemType);
			}

			for(const auto& entityAccessIds: entitiesWithAccessIds){
				systemTypeIndexMap[systemType]->entityComponentReferenceMap[entityAccessIds.first] = entityAccessIds.second;
			}
		}

		void removeEntityFromSystem(Entity entityToRemove, Signature signatureOfSystem){

			auto systemsAssociatedWithEntity = getSystemsContainingSignature(signatureOfSystem);

			for(auto& systemId: systemsAssociatedWithEntity) {
				systemTypeIndexMap[systemId]->entityComponentReferenceMap.erase(entityToRemove);
			}
			assignedEntitySystemMap.erase(entityToRemove);
		}



		/// Remove an entity from all systems that are associated with this one.
		/// \param entity The entity to remove from all systems.
		void removeEntityFromSystems(Entity& entity){
			for(auto& system_type : assignedEntitySystemMap[entity]){
				systemTypeIndexMap[system_type]->entityComponentReferenceMap.erase(entity);
			}
			assignedEntitySystemMap.erase(entity);
		}

		/// Get a system as a pointer
		/// \param systemTypeIndex The type index of the requested system.
		/// \return Optional pointer to the system, depending on its existence in the manager.
		std::optional<System*> getSystem(std::type_index systemTypeIndex){
			if(!isSystemRegistred(systemTypeIndex)) return std::nullopt;
			return std::make_optional<System*>(systemTypeIndexMap[systemTypeIndex].get());
		}

		std::vector<std::type_index> getSystemsContainingSignature(Signature signature){
			auto systemIds = std::vector<std::type_index>();
			for(const auto& systemSignature: systemSignatureMap){
				if((signature & systemSignature.second) != signature)continue;
				systemIds.push_back(systemSignature.first);
			}
			return systemIds;
		}

//		void setSystemExecutionOrder();

	private:
		std::unordered_map<std::type_index, std::unique_ptr<System>> systemTypeIndexMap;
		std::unordered_map<std::type_index, Signature> systemSignatureMap;

		std::unordered_map<Entity, std::vector<std::type_index>> assignedEntitySystemMap;
//		std::vector<System> lateUpdateSystems;
//		std::vector<System> renderSystems;

		bool isSystemRegistred(std::type_index systemTypeID){
			return systemTypeIndexMap.contains(systemTypeID);
		}

		friend class WorldFriendAccessor;

};

#endif //JAREP_SYSTEMMANAGER_HPP
