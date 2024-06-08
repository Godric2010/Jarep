cmake_minimum_required(VERSION 3.25)

set(IRENDERER_API_HEADERS IRenderAPI.hpp NativeWindowHandleProvider.hpp)
set(IRENDERER_API_SOURCES stb_impl.cpp)

add_library(IRendererAPI STATIC ${IRENDERER_API_HEADERS} ${IRENDERER_API_SOURCES})
target_include_directories(IRendererAPI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(stb REQUIRED)
target_link_libraries(IRendererAPI PUBLIC stb::stb)

add_library(STB_IMPL STATIC stb_impl.cpp)
target_include_directories(STB_IMPL PRIVATE $<TARGET_PROPERTY:stb::stb,INTERFACE_INCLUDE_DIRECTORIES>)
target_link_libraries(STB_IMPL PRIVATE stb::stb)

target_link_libraries(IRendererAPI PUBLIC STB_IMPL)
