cmake_minimum_required(VERSION 3.25)


set(SOURCE_FILES JarepGraphics.cpp)

if(APPLE)
    list(APPEND SOURCE_FILES metal-api/metalapi.cpp)
elseif (UNIX OR WIN32)
    list(APPEND SOURCE_FILES vulkan-api/vulkanapi.cpp)
endif ()


add_library(JAREP_RENDERER JarepGraphics.hpp ${SOURCE_FILES})

if(APPLE)
    find_library(METAL_LIBRARY Metal)
    target_link_libraries(JAREP_RENDERER PRIVATE ${METAL_LIBRARY})
elseif (UNIX OR WIN32)
    find_package(Vulkan REQUIRED)
    target_link_libraries(JAREP_RENDERER PRIVATE Vulkan::Vulkan)
endif ()


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
