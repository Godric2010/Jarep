cmake_minimum_required(VERSION 3.25)


set(SOURCE_FILES JarepGraphics.cpp
        NativeWindowHandleProvider.hpp
        Vertex.hpp)

if(APPLE)
    list(APPEND SOURCE_FILES metal-api/metalapi.cpp)
elseif (UNIX OR WIN32)
    list(APPEND SOURCE_FILES vulkan-api/vulkanapi.cpp)
endif ()


add_library(JAREP_RENDERER JarepGraphics.hpp ${SOURCE_FILES})

if(APPLE)
    add_subdirectory(metal-api/metal)
    target_link_libraries(JAREP_RENDERER PUBLIC METAL_CPP)
elseif (UNIX OR WIN32)
    find_package(Vulkan REQUIRED)
    target_link_libraries(JAREP_RENDERER PUBLIC Vulkan::Vulkan)
endif ()

find_package(glm)
target_link_libraries(JAREP_RENDERER PUBLIC glm::glm)

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
