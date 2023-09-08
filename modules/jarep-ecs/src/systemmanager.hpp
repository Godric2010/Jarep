//
// Created by Sebastian Borsch on 10.08.23.
//

#ifndef JAREP_SYSTEMMANAGER_HPP
#define JAREP_SYSTEMMANAGER_HPP

#include <utility>
#include <vector>
#include <optional>
#include <typeindex>
#include "system.hpp"

class SystemManager {

	public:
		SystemManager() = default;

		~SystemManager() = default;


		template<class T, class = typename std::enable_if<std::is_base_of<System, T>::value>::type>
		std::optional<std::type_index> registerSystem(Signature systemSignature, std::shared_ptr<GetComponentsFunc> getComponentsFunc) {

			if (systemTypeIndexMap.contains(typeid(T))) return std::nullopt;
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

		void setSystemExecutionOrder();

	private:
		std::unordered_map<std::type_index, std::unique_ptr<System>> systemTypeIndexMap;
		std::unordered_map<std::type_index, Signature> systemSignatureMap;
//		std::vector<System> lateUpdateSystems;
//		std::vector<System> renderSystems;
};

#endif //JAREP_SYSTEMMANAGER_HPP
