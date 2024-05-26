cmake_minimum_required(VERSION 3.25)

set(HEADER_FILES metalapi.hpp sdlsurfaceadapter.hpp
        MetalCommandQueueBuilder.hpp)
set(SOURCE_FILES metalapi.cpp sdlsurfaceadapter.mm
        MetalCommandQueueBuilder.cpp)

add_library(METAL_RENDERER_API ${SOURCE_FILES} ${HEADER_FILES})

add_subdirectory(metal)
target_link_libraries(METAL_RENDERER_API PUBLIC METAL_CPP)
target_link_libraries(METAL_RENDERER_API PUBLIC IRendererAPI)

set_target_properties(METAL_RENDERER_API PROPERTIES HEADER_FILES "${HEADER_FILES}")
target_include_directories(METAL_RENDERER_API PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
install(TARGETS METAL_RENDERER_API
        EXPORT METAL_RENDERER_API_Export
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        INCLUDES DESTINATION include
)


