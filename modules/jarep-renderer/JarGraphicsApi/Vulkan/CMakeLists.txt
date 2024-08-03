project(VulkanLibrary)

set(SOURCE_FILES VulkanBackend.cpp)

find_package(Vulkan REQUIRED)
set(PLATFORM_LIBS ${Vulkan_LIBRARIES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${Vulkan_INCLUDE_DIRS})

add_library(vulkan_api ${SOURCE_FILES})
target_link_libraries(JAR_GRAPHICS_API vulkan_api ${Vulkan_LIBRARIES})