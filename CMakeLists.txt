cmake_minimum_required(VERSION 3.16)
project(JAREP)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

add_subdirectory(src/WindowManagement)

add_executable(JAREP main.cpp)

target_link_libraries(JAREP
        PRIVATE WindowManagement
)

option(ENABLE_TESTS "Enable building tests" ON)
if(ENABLE_TESTS)
    enable_testing()
endif ()