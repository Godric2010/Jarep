cmake_minimum_required(VERSION 3.25)

set(IRENDERER_API_HEADERS IRenderAPI.hpp NativeWindowHandleProvider.hpp)

add_library(IRendererAPI INTERFACE)
target_include_directories(IRendererAPI INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
        $<TARGET_PROPERTY:stb::stb,INTERFACE_INCLUDE_DIRECTORIES>
)

find_package(stb REQUIRED)
target_link_libraries(IRendererAPI INTERFACE stb::stb)

add_library(STB_IMPL STATIC stb_impl.cpp)
target_include_directories(STB_IMPL PRIVATE $<TARGET_PROPERTY:stb::stb,INTERFACE_INCLUDE_DIRECTORIES>)

target_link_libraries(IRendererAPI INTERFACE STB_IMPL)
