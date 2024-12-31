cmake_minimum_required(VERSION 3.16)

project(WindowManagement LANGUAGES CXX)

set(LIBRARY_NAME WindowManagement)
set(LIBRARY_VERSION 0.1)

find_package(SDL2 REQUIRED)

add_library(${LIBRARY_NAME}
        include/${LIBRARY_NAME}/IWindowManager.hpp
        include/WindowManagement/WindowSettings.hpp
        src/SdlWindowManager.cpp
        src/SdlWindowManager.hpp
        include/WindowManagement/WindowCreator.hpp
)

target_include_directories(${LIBRARY_NAME} PUBLIC include)
target_link_libraries(${LIBRARY_NAME} PRIVATE SDL2::SDL2)

set_target_properties(${LIBRARY_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

find_package(Doxygen REQUIRED)
if (DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    set(DOXYGEN_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/docs)
    set(DOXYGEN_XML_DIR ${DOXYGEN_OUT_DIR}/xml)
    set(DOXYGEN_OUTPUT_FILE ${DOXYGEN_OUT_DIR}/API.md)

    file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_command(
            TARGET ${LIBRARY_NAME}
            POST_BUILD
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            COMMAND python3 ${CMAKE_SOURCE_DIR}/markdown_docu_builder.py ${DOXYGEN_XML_DIR} ${DOXYGEN_OUTPUT_FILE}
            COMMENT "Generating Markdown API documentation for ${LIBRARY_NAME}"
    )

endif ()