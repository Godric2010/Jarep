add_library(JAREP_ECS
        archetype.cpp
        archetype.hpp
        componentInstanceCollection.hpp
        entitymanager.cpp
        entitymanager.hpp
        World.cpp
        World.hpp
        entitymanager.cpp Component.hpp
        System.hpp
        componentmanager.hpp
        signature.hpp)

target_include_directories(JAREP_ECS PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})