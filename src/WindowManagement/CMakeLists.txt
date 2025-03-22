cmake_minimum_required(VERSION 3.16)

project(WindowManagement LANGUAGES CXX)

set(LIBRARY_NAME WindowManagement)
set(LIBRARY_VERSION 0.1.0)

include(${CMAKE_SOURCE_DIR}/cmake/Helpers.cmake)
find_package(SDL2 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

add_library(${LIBRARY_NAME}
        include/${LIBRARY_NAME}/IWindowManager.hpp
        include/WindowManagement/WindowSettings.hpp
        src/SdlWindowManager.cpp
        src/SdlWindowManager.hpp
        include/WindowManagement/WindowCreator.hpp
        include/WindowManagement/DisplayMode.hpp
        include/WindowManagement/IWindowHandle.hpp
)

target_include_directories(${LIBRARY_NAME} PUBLIC include ${SDL2_INCLUDE_DIRS})
target_link_libraries(${LIBRARY_NAME} PUBLIC SDL2::SDL2 Vulkan::Vulkan)

set_target_properties(${LIBRARY_NAME} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

generate_doxygen_with_markdown(${LIBRARY_NAME})
add_library_tests(${LIBRARY_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/tests)