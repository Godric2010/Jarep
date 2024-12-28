cmake_minimum_required(VERSION 3.16)

project(WindowManagement LANGUAGES CXX)

set(LIBRARY_NAME WindowManagement)
set(LIBRARY_VERSION 0.1)

add_library(${LIBRARY_NAME}
        include/${LIBRARY_NAME}/${LIBRARY_NAME}.hpp
        src/${LIBRARY_NAME}.cpp
)

target_include_directories(${LIBRARY_NAME} PUBLIC include)

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

    add_custom_target(doxygen_xml ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generation API documentation for ${LIBRARY_NAME} with doxygen"
            VERBATIM
    )

    add_custom_command(
            OUTPUT ${DOXYGEN_OUTPUT_FILE}
            COMMAND python3 ${CMAKE_SOURCE_DIR}/markdown_docu_builder.py ${DOXYGEN_XML_DIR} ${DOXYGEN_OUTPUT_FILE}
            DEPENDS doxygen_xml
            COMMENT "Generating Markdown API documentation for ${LIBRARY_NAME}"
    )

    add_custom_target(${LIBRARY_NAME}_doc ALL DEPENDS ${DOXYGEN_OUTPUT_FILE})
endif ()