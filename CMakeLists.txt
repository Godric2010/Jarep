cmake_minimum_required(VERSION 3.30)
project(JAREP)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

add_subdirectory(src/WindowManagement)

#find_package(SDL2 REQUIRED)
#find_package(Vulkan REQUIRED)

add_executable(JAREP main.cpp)

target_link_libraries(JAREP
        PRIVATE WindowManagement
)