cmake_minimum_required(VERSION 3.25)

project(JarGraphicsLibrary)

set(SOURCE_FILES JarBackend.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library(JAR_GRAPHICS_API STATIC ${SOURCE_FILES})

if(APPLE)
    message(STATUS "Building for Apple (macOS)")
    add_subdirectory(Metal)
    add_definitions(-DPLATFORM_APPLE)
elseif(WIN32 OR UNIX)
    message(STATUS "Building for Windows/Linux")
    add_subdirectory(Vulkan)
    add_definitions(-DPLATFORM_VULKAN)
else ()
    message(FATAL_ERROR "Unsupported platform")
endif ()

target_link_libraries(JAR_GRAPHICS_API ${PLATFORM_LIBS})



