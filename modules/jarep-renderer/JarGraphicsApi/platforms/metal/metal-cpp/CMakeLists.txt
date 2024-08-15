
# Library definition
add_library(METAL_CPP
        ${CMAKE_CURRENT_SOURCE_DIR}/definition.cpp
)

# Metal cpp headers
target_include_directories(METAL_CPP PUBLIC
        "metal-cpp"
        "metal-cpp-extensions"
)

# Metal cpp library (linker)
target_link_libraries(METAL_CPP
        "-framework Metal"
        "-framework Foundation"
        "-framework AppKit"
        "-framework MetalKit"
        "-framework QuartzCore"
)