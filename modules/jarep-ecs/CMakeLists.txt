cmake_minimum_required(VERSION 3.25)
add_library(jarep-ecs include/World.hpp src/World.cpp src/archetype.hpp src/componentInstanceCollection.hpp src/entitygenerator.hpp src/entitygenerator.cpp src/archetype.cpp)

find_package(Catch2 REQUIRED)
add_executable(jarep-ecs-tests tests/entitygeneratortests.cpp tests/archetypetests.cpp)

target_link_libraries(jarep-ecs-tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(jarep-ecs-tests PRIVATE jarep-ecs)

enable_testing()
add_test(NAME jarep-ecs-test
        COMMAND $<TARGET_FILE:tests/entitygeneratortests> --success)