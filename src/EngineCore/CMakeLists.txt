cmake_minimum_required(VERSION 3.16)

project(EngineCore LANGUAGES CXX)

set(LIBRARY_NAME EngineCore)
set(LIBRARY_VERSION 0.1.0)

include(${CMAKE_SOURCE_DIR}/cmake/Helpers.cmake)

find_package(glm REQUIRED)

add_library(${LIBRARY_NAME})

target_include_directories(${LIBRARY_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(${LIBRARY_NAME} PRIVATE
        include/${LIBRARY_NAME}/Types/Vertex.hpp
        include/${LIBRARY_NAME}/Types/Mesh.hpp
        include/${LIBRARY_NAME}/IMeshLibrary.hpp
        src/MeshLibrary.cpp
        src/Hashing/TypeHasher.hpp
)


target_link_libraries(${LIBRARY_NAME} PRIVATE glm::glm)

set_target_properties(${LIBRARY_NAME} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

generate_doxygen_with_markdown(${LIBRARY_NAME})
add_library_tests(${LIBRARY_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/tests)
