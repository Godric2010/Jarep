cmake_minimum_required(VERSION 3.25)

set(HEADER_FILES vulkanapi.hpp sdlsurfaceadapter.hpp)
set(SOURCE_FILES vulkanapi.cpp sdlsurfaceadapter.cpp)

add_library(VULKAN_RENDERER_API STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_compile_definitions(VULKAN_RENDERER_API PRIVATE BUILDING_DLL)

find_package(Vulkan REQUIRED)
target_link_libraries(VULKAN_RENDERER_API PUBLIC Vulkan::Vulkan IRendererAPI)

target_include_directories(VULKAN_RENDERER_API PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>)

install(TARGETS VULKAN_RENDERER_API
        EXPORT VULKAN_RENDERER_API_Export
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        INCLUDES DESTINATION include
)
