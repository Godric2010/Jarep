cmake_minimum_required(VERSION 3.25)
add_library(JAREP_RENDERER JarepGraphics.hpp
        IRenderer.hpp y.cpp)

set(PublicHeaders JarepGraphics.hpp)
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
