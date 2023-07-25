cmake_minimum_required(VERSION 3.25)

add_executable(JAREP_ECS_Tests
        archetypetests.cpp
        entitygeneratortests.cpp
        )

find_package(Catch2 REQUIRED)
target_link_libraries(JAREP_ECS_Tests PUBLIC JAREP_ECS Catch2::Catch2WithMain)

# Enable verbose output for Catch2 tests
target_compile_definitions(JAREP_ECS_Tests PUBLIC CATCH_CONFIG_VERBOSE)

# Set the output directory for the test executable
set_target_properties(JAREP_ECS_Tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test)

enable_testing()

add_test(NAME JAREP_ECS COMMAND $<TARGET_FILE:JAREP_ECS_Tests> --success)
