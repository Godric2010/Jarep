cmake_minimum_required(VERSION 3.25)
add_library(jarep-ecs include/World.hpp src/World.cpp src/archetype.hpp src/component.hpp src/entitygenerator.hpp src/entitygenerator.cpp)