cmake_minimum_required(VERSION 3.25)

project(JarGraphicsAPI)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

add_library(JAR_GRAPHICS_API STATIC ${SOURCE_FILES})

target_include_directories(JAR_GRAPHICS_API PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

if (APPLE)
    message(STATUS "Building for Apple (macOS)")
    add_subdirectory(platforms/metal)
    target_link_libraries(JAR_GRAPHICS_API PRIVATE METAL_GRAPHICS_API)
    add_definitions(-DPLATFORM_APPLE)
elseif (WIN32 OR UNIX)
    message(STATUS "Building for Windows/Linux")
    add_subdirectory(platforms/vulkan)
    target_link_libraries(JAR_GRAPHICS_API PRIVATE VULKAN_GRAPHICS_API)
    add_definitions(-DPLATFORM_VULKAN)
else ()
    message(FATAL_ERROR "Unsupported platform")
endif ()

set_target_properties(JAR_GRAPHICS_API PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN YES
)

install(TARGETS JAR_GRAPHICS_API
        EXPORT JarGraphicsAPIConfig
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)
