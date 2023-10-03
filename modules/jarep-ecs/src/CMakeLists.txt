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
        systemmanager.hpp
        entity.hpp)

set(PUBLIC_HEADERS
    world.hpp
    entity.hpp
    component.hpp
    system.hpp
)

set_target_properties(JAREP_ECS PROPERTIES PUBLIC_HEADERS "${PUBLIC_HEADERS}")

target_include_directories(JAREP_ECS PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

install(TARGETS JAREP_ECS
    EXPORT JAREP_ECS_Export
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)