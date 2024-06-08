cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SOURCE_FILES IRendererAPI/stb_impl.cpp)

add_subdirectory(IRendererAPI)
add_library(JAREP_RENDERER_API STATIC ${SOURCE_FILES})
target_link_libraries(JAREP_RENDERER_API PUBLIC IRendererAPI)

if (APPLE)
    add_subdirectory(metal-api)
    target_link_libraries(JAREP_RENDERER_API PUBLIC METAL_RENDERER_API)
else ()
    add_subdirectory(vulkan-api)
    target_link_libraries(JAREP_RENDERER_API PUBLIC VULKAN_RENDERER_API)
endif ()

target_include_directories(JAREP_RENDERER_API PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/IRendererAPI>
        <INSTALL_INTERFACE:include>
)
