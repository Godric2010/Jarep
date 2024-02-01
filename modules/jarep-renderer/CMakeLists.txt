cmake_minimum_required(VERSION 3.25)


set(SOURCE_FILES JarepGraphics.cpp
        NativeWindowHandleProvider.hpp
        Vertex.hpp
        vulkan-api/sdlsurfaceadapter.cpp
        vulkan-api/sdlsurfaceadapter.hpp
)

if(APPLE)
    list(APPEND SOURCE_FILES metal-api/metalapi.cpp metal-api/sdlsurfaceadapter.hpp metal-api/sdlsurfaceadapter.mm)
elseif (UNIX OR WIN32 OR WIN64)
    list(APPEND SOURCE_FILES vulkan-api/vulkanapi.hpp vulkan-api/vulkanapi.cpp vulkan-api/sdlsurfaceadapter.hpp vulkan-api/sdlsurfaceadapter.cpp)
endif ()


add_library(JAREP_RENDERER JarepGraphics.hpp ${SOURCE_FILES})

if(APPLE)
    add_subdirectory(metal-api/metal)
    target_link_libraries(JAREP_RENDERER PUBLIC METAL_CPP)
elseif (UNIX OR WIN32 OR WIN64)
    find_package(Vulkan REQUIRED)
    target_link_libraries(JAREP_RENDERER PUBLIC Vulkan::Vulkan)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_definitions(-DENABLE_VALIDATION_LAYERS)
    endif ()
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
