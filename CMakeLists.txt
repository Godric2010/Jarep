cmake_minimum_required(VERSION 3.25)
project(JAREP)

add_subdirectory(modules/jarep-ecs)
add_subdirectory(modules/jarep-renderer)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE jarep-ecs)
target_link_libraries(${PROJECT_NAME} PRIVATE jarep-renderer)