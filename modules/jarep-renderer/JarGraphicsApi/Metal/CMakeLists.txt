project(MetalLibrary)

set(SOURCE_FILES MetalBackend.cpp)

set(PLATFORM_LIBS ${Metal_LIBRARIES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${Metal_INCLUDE_DIRS})

add_library(metal_api ${SOURCE_FILES})
target_link_libraries(JAR_GRAPHICS_API metal_api)
