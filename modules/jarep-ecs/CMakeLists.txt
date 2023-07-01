cmake_minimum_required(VERSION 3.25)
add_library(jarep-ecs include/World.hpp src/World.cpp src/archetype.hpp src/component.hpp src/entitygenerator.hpp src/entitygenerator.cpp)

find_package(Catch2 3 REQUIRED)
add_executable(jarep-ecs-tests tests/entitygeneratortests.cpp)

target_link_libraries(jarep-ecs-tests PRIVATE Catch2::Catch2WithMain)