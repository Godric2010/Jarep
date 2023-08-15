add_library(JAREP_ECS
        archetype.cpp
        archetype.hpp
        componentInstanceCollection.hpp
        entitymanager.cpp
        entitymanager.hpp
        world.hpp
        entitymanager.cpp component.hpp
        system.hpp
        componentmanager.hpp
        signature.hpp
        systemmanager.hpp)

target_include_directories(JAREP_ECS PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})