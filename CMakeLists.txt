cmake_minimum_required(VERSION 3.16)
project(JAREP)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)
set(CMAKE_VERBOSE_MAKEFILE ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}" /cmake)

add_subdirectory(src/WindowManagement)
add_subdirectory(src/Rendering)

add_executable(JAREP main.cpp)
add_dependencies(JAREP CompileShaders)

target_link_libraries(JAREP PRIVATE
        WindowManagement
        Rendering
)

find_package(Python3 REQUIRED)
file(GLOB_RECURSE SHADER_SOURCES
        "${CMAKE_SOURCE_DIR}/shaders/*.vert"
        "${CMAKE_SOURCE_DIR}/shaders/*.frag"
)

add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/shaders/compiled.flag
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/compile_shaders.py
            --output=${CMAKE_BINARY_DIR}/shaders/compiled.flag
        DEPENDS ${SHADER_SOURCES}
        COMMENT "Compiling Vulkan Shaders"
)

add_custom_target(CompileShaders ALL DEPENDS ${CMAKE_BINARY_DIR}/shaders/compiled.flag)

option(ENABLE_TESTS "Enable building tests" ON)
if (ENABLE_TESTS)
    enable_testing()
endif ()