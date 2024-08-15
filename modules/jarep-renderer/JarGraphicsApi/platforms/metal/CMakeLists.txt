file(GLOB_RECURSE METAL_SOURCES src/*.cpp)
file(GLOB_RECURSE METAL_HEADERS include/*.hpp)

add_library(METAL_GRAPHICS_API STATIC
        ${METAL_SOURCES}
        ${METAL_HEADERS}
)
add_subdirectory(metal)
target_link_libraries(METAL_GRAPHICS_API PUBLIC METAL_CPP)

target_include_directories(METAL_GRAPHICS_API PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(METAL_GRAPHICS_API PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN YES
)