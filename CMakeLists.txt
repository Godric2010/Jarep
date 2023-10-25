cmake_minimum_required(VERSION 3.25)
if (APPLE)
    project(JAREP CXX OBJCXX)
else ()
    project(JAREP CXX)
endif ()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64")

set(CMAKE_CXX_FLAGS_DEBUG "-g")

add_subdirectory(modules)
add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE JAREP_CORE)
