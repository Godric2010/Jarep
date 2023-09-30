//
// Created by Sebastian Borsch on 30.07.23.
//

#ifndef JAREP_SYSTEM_HPP
#define JAREP_SYSTEM_HPP

#include <memory>
#include <utility>
#include <vector>
#include <typeindex>
#include <unordered_map>
#include <functional>
#include <tuple>
#include "systemmanager.hpp"
#include "componentmanager.hpp"

class System {


	public:

		System() = default;

		virtual ~System() = default;

	protected:

		virtual void update() = 0;

		template<typename T>
		std::optional<std::shared_ptr<T>> getComponent(Entity entity) {
			auto componentRef = entityComponentReferenceMap.at(entity);
			Signature entitySignature = std::get<0>(componentRef);
			size_t entityArchetypeIndex = std::get<1>(componentRef);

			auto componentResult = getComponentFunc->template operator()<T>(entitySignature, entityArchetypeIndex);
			if (!componentResult.has_value()) {
				return std::nullopt;
			}

			auto component = std::dynamic_pointer_cast<T>(componentResult.value());
			return component ? std::make_optional(component) : std::nullopt;
		}

		std::vector<Entity> getEntities() const {
			std::vector<Entity> entities;
			for (const auto &pair: entityComponentReferenceMap) {
				entities.push_back(pair.first);
			}
			return entities;
		}


	private:
		std::unordered_map<Entity, std::tuple<Signature, size_t>> entityComponentReferenceMap;
		std::shared_ptr<GetComponentsFunc> getComponentFunc;

		friend class SystemManager;
		friend class WorldFriendAccessor;
};

#endif //JAREP_SYSTEM_HPP
