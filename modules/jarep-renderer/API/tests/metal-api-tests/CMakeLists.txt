cmake_minimum_required(VERSION 3.25)

add_executable(METAL_API_Tests
        MetalCommandQueueBuilderTests.cpp
)

find_package(Catch2 REQUIRED)
target_link_libraries(METAL_API_Tests PUBLIC METAL_RENDERER_API Catch2::Catch2WithMain)

# Enable verbose output for Catch2 tests
target_compile_definitions(METAL_API_Tests PUBLIC CATCH_CONFIG_VERBOSE)

# Set the output directory for the test executable
set_target_properties(METAL_API_Tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test)

enable_testing()

add_test(NAME METAL_API COMMAND $<TARGET_FILE:METAL_API_Tests> --success)
