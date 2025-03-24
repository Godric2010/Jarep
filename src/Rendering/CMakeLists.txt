cmake_minimum_required(VERSION 3.16)

project(Rendering LANGUAGES CXX)

set(LIBRARY_NAME Rendering)
set(LIBRARY_VERSION 0.1.0)

include(${CMAKE_SOURCE_DIR}/cmake/Helpers.cmake)
find_package(Vulkan REQUIRED)

add_library(${LIBRARY_NAME}
        include/${LIBRARY_NAME}/IRenderer.hpp
        include/${LIBRARY_NAME}/RendererCreator.hpp
        src/RenderManager.cpp
        src/core/VulkanCore.cpp
        src/core/VulkanInstance.cpp
        src/core/VulkanSurface.cpp
)

target_include_directories(${LIBRARY_NAME} PUBLIC include ${VULKAN_INCLUDE_DIRS})
target_link_libraries(${LIBRARY_NAME} PRIVATE Vulkan::Vulkan)

set_target_properties(${LIBRARY_NAME} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

generate_doxygen_with_markdown(${LIBRARY_NAME})
add_library_tests(${LIBRARY_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/tests)
