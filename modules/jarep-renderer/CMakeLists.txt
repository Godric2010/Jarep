cmake_minimum_required(VERSION 3.25)


set(SOURCE_FILES JarepGraphics.cpp)

if(APPLE)
    list(APPEND SOURCE_FILES metal-api/metalapi.mm)
elseif (UNIX OR WIN32)
    list(APPEND SOURCE_FILES vulkan-api/vulkanapi.cpp)
endif ()


add_library(JAREP_RENDERER JarepGraphics.hpp ${SOURCE_FILES})

if(APPLE)
    find_library(METAL_LIBRARY Metal)
    find_library(FOUNDATION_LIBRARY Foundation)
    target_link_libraries(JAREP_RENDERER PRIVATE ${METAL_LIBRARY} ${FOUNDATION_LIBRARY})
    set_source_files_properties(metal-api/metalapi.mm PROPERTIES LANGUAGE OBJCXX)
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
