//
// Created by Sebastian Borsch on 30.07.23.
//

#ifndef JAREP_SYSTEM_HPP
#define JAREP_SYSTEM_HPP

#include <memory>
#include <vector>
#include <typeindex>
#include "componentmanager.hpp"
#include "systemmanager.hpp"

class System{

		friend class SystemManager;
	public:
        System(std::vector<std::type_index> requiredComponentTypes){
			requiredComponents = requiredComponentTypes;
		}
        virtual ~System() = default;

	protected:

		std::vector<Entity> entities;

		virtual void update() = 0;


	private:

		std::vector<std::type_index> requiredComponents;

};

#endif //JAREP_SYSTEM_HPP
