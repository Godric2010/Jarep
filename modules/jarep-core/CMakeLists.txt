cmake_minimum_required(VERSION 3.25)

add_subdirectory(Window)

add_library(JAREP_CORE
        Window/IWindow.hpp
        core.cpp
        core.hpp
        Window/sdlwindow.cpp
        Window/sdlwindow.hpp)

find_package(SDL2 REQUIRED)

set(PUBLIC_HEADERS core.hpp Window/IWindow.hpp)
set_target_properties(JAREP_CORE PROPERTIES PUBLIC_HEADERS "${PUBLIC_HEADERS}")

include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(JAREP_CORE PUBLIC ${SDL2_LIBRARIES})
target_link_libraries(JAREP_CORE PUBLIC JAREP_RENDERER)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
message("Includes of CORE: ${dirs}")


target_include_directories(JAREP_CORE PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

install(TARGETS JAREP_CORE
        EXPORT JAREP_CORE_Export
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)
