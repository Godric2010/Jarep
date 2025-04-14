cmake_minimum_required(VERSION 3.16)

project(AssetLoader LANGUAGES CXX)

set(LIBRARY_NAME AssetLoader)
set(LIBRARY_VERSION 0.1.0)

include(${CMAKE_SOURCE_DIR}/cmake/Helpers.cmake)

find_package(tinyobjloader CONFIG REQUIRED)
add_library(${LIBRARY_NAME})

target_include_directories(${LIBRARY_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(${LIBRARY_NAME} PRIVATE
        include/${LIBRARY_NAME}/IMeshLoader.hpp
        src/MeshLoader.cpp

)


target_link_libraries(${LIBRARY_NAME}
        PUBLIC EngineCore
        PRIVATE tinyobjloader::tinyobjloader)

set_target_properties(${LIBRARY_NAME} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

generate_doxygen_with_markdown(${LIBRARY_NAME})
#add_library_tests(${LIBRARY_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/tests)
