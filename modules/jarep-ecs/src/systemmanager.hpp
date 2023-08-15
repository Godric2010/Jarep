//
// Created by Sebastian Borsch on 10.08.23.
//

#ifndef JAREP_SYSTEMMANAGER_HPP
#define JAREP_SYSTEMMANAGER_HPP

#include <vector>
#include <optional>
#include <typeindex>
#include "system.hpp"

class SystemManager {

	public:
		SystemManager() = default;
		~SystemManager() = default;


		template<class T, class = typename std::enable_if<std::is_base_of<System, T>::value>::type>
		std::optional<std::pair<std::type_index, std::vector<std::type_index>>> registerSystem(std::vector<std::type_index> requiredComponents) {

			if(systemTypeIndexMap.contains(typeid(T))) return std::nullopt;
			std::unique_ptr<System> system = std::make_unique<T>(requiredComponents);

			systemTypeIndexMap.insert_or_assign(typeid(T), std::move(system));
			systemComponentsMap.insert_or_assign(typeid(T), requiredComponents);
			return std::make_optional( std::make_pair(std::type_index(typeid(T)), requiredComponents));
		}

		template<class T, class = typename std::enable_if<std::is_base_of<System, T>::value>::type>
		void unregisterSystem() {

			if(!systemTypeIndexMap.contains(typeid(T))) return;
			systemTypeIndexMap.erase(typeid(T));
			systemComponentsMap.erase(typeid(T));

		}

		void update();

		void updateSystemData(std::type_index systemType, Signature componentSignature, std::vector<std::tuple<Entity, size_t>> entities){

			if(!systemTypeIndexMap.contains(systemType)) return;


		}

		void setSystemExecutionOrder();

	private:
		std::unordered_map<std::type_index, std::unique_ptr<System>> systemTypeIndexMap;
		std::unordered_map<std::type_index, std::vector<std::type_index>> systemComponentsMap;
//		std::vector<System> lateUpdateSystems;
//		std::vector<System> renderSystems;
};

#endif //JAREP_SYSTEMMANAGER_HPP
