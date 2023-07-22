add_library(JAREP_ECS
        archetype.cpp
        archetype.hpp
        componentInstanceCollection.hpp
        entity-generator.cpp
        entity-generator.hpp
        World.cpp
        World.hpp
        entity-generator.cpp Component.hpp)

target_include_directories(JAREP_ECS PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})