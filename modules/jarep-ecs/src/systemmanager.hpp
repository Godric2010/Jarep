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
			system->getComponents = std::move(getComponentsFunc);

			systemTypeIndexMap.insert_or_assign(typeid(T), std::move(system));
			systemSignatureMap.insert_or_assign(typeid(T), systemSignature);
			return std::make_optional(std::type_index(typeid(T)));
		}

		template<class T, class = typename std::enable_if<std::is_base_of<System, T>::value>::type>
		void unregisterSystem() {

			if (!systemTypeIndexMap.contains(typeid(T))) return;
			systemTypeIndexMap.erase(typeid(T));
			systemSignatureMap.erase(typeid(T));

		}

		void update() {
			for (auto &system: systemTypeIndexMap) {
				system.second->update();
			}
		}

		void updateSystemData(std::type_index systemType,
		                      std::unordered_map<Entity, std::tuple<Signature, size_t>> entitiesWithAccessIds) {

			if (!systemTypeIndexMap.contains(systemType)) return;
			systemTypeIndexMap.at(systemType)->entityComponentReferenceMap = std::move(entitiesWithAccessIds);


		}

		std::optional<System*> getSystem(std::type_index systemTypeIndex){
			if(!isSystemRegistred(systemTypeIndex)) return std::nullopt;
			return std::make_optional<System*>(systemTypeIndexMap[systemTypeIndex].get());
		}


		void setSystemExecutionOrder();

	private:
		std::unordered_map<std::type_index, std::unique_ptr<System>> systemTypeIndexMap;
		std::unordered_map<std::type_index, Signature> systemSignatureMap;
//		std::vector<System> lateUpdateSystems;
//		std::vector<System> renderSystems;

		bool isSystemRegistred(std::type_index systemTypeID){
			return systemTypeIndexMap.contains(systemTypeID);
		}

};

#endif //JAREP_SYSTEMMANAGER_HPP
