cmake_minimum_required(VERSION 3.25)
project(JAREP)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(modules/jarep-ecs)
add_subdirectory(modules/jarep-renderer)

find_package(Catch2 REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2)

target_link_libraries(${PROJECT_NAME} PRIVATE jarep-ecs)
target_link_libraries(${PROJECT_NAME} PRIVATE jarep-renderer)