cmake_minimum_required(VERSION 3.25)

set(SOURCE_FILES JarRenderer.cpp
        Vertex.hpp
        Mesh.hpp
        JarRenderStep.cpp
        JarRenderStep.hpp
)



add_subdirectory(API)
add_library(JAREP_RENDERER JarRenderer.hpp ${SOURCE_FILES})
target_link_libraries(JAREP_RENDERER PUBLIC JAREP_RENDERER_API)

find_package(glm)
target_link_libraries(JAREP_RENDERER PUBLIC glm::glm)

set(PublicHeaders JarRenderer.hpp)
set_target_properties(JAREP_RENDERER PROPERTIES PUBLIC_HEADERS "${PublicHeaders}")
target_include_directories(JAREP_RENDERER PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

install(TARGETS JAREP_RENDERER
        EXPORT JAREP_RENDERER_Export
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)
