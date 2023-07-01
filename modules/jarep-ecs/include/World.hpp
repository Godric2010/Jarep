//
// Created by Sebastian Borsch on 01.07.23.
//

#ifndef JAREP_WORLD_HPP
#define JAREP_WORLD_HPP


#include <iostream>
#include "../src/entitygenerator.hpp"


class World {
    public:
        World() {

        }

        ~World() {

        }

        Entity createNewEntity();
        void removeEntity();



    private:

};


#endif //JAREP_WORLD_HPP
