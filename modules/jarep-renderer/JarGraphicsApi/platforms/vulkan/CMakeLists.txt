project(VulkanGraphicsAPI)

file(GLOB_RECURSE VULKAN_SOURCES src/*.cpp)
file(GLOB_RECURSE VULKAN_HEADERS include/*.hpp)

add_library(VULKAN_GRAPHICS_API STATIC
        ${VULKAN_SOURCES}
        ${VULKAN_HEADERS}
)

find_package(Vulkan REQUIRED)
target_link_libraries(VULKAN_GRAPHICS_API PUBLIC Vulkan::Vulkan)

target_include_directories(VULKAN_GRAPHICS_API PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(VULKAN_GRAPHICS_API PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN YES
)
