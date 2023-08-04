//
// Created by sebastian on 04.08.23.
//

#ifndef JAREP_COMPONENTMANAGER_HPP
#define JAREP_COMPONENTMANAGER_HPP

#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <type_traits>
#include "signature.hpp"
#include "Component.hpp"

class ComponentManager {

    public:
        ComponentManager() {
            nextComponentType = 0;
            componentBitMap = std::unordered_map<std::type_index, std::size_t>();
        }
        ~ComponentManager() = default;

        template<class T, class = class std::enable_if<std::is_base_of<Component, T>::value>::type>
        void regsisterComponent(){
            if(nextComponentType >= MAX_COMPONENTS){
                return;
            }
            componentBitMap[std::type_index(typeid(T))] = nextComponentType;
            ++nextComponentType;
        }

    private:
        std::unordered_map<std::type_index, std::size_t> componentBitMap;
        std::size_t nextComponentType;
};


#endif //JAREP_COMPONENTMANAGER_HPP
