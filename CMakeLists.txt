cmake_minimum_required(VERSION 3.25)
if (APPLE)
    project(JAREP CXX OBJCXX)
else ()
    project(JAREP CXX)
endif ()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64")

#set(CMAKE_CXX_FLAGS_DEBUG "-g")
enable_testing()

add_subdirectory(modules)
add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE JAREP_CORE)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_SOURCE_DIR}/compileShaders.sh ARGS ${CMAKE_BINARY_DIR}
                    COMMENT "Run shader compilation as post build step")

add_custom_target(run_all_tests COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
                    COMMENT "Run all tests")