# cmake/Helpers.cmake

function(generate_doxygen_with_markdown target_name)
    find_package(Doxygen REQUIRED)

    if(DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        set(DOXYGEN_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/docs)
        set(DOXYGEN_XML_DIR ${DOXYGEN_OUT_DIR}/xml)
        set(DOXYGEN_OUTPUT_FILE ${DOXYGEN_OUT_DIR}/API.md)

        file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

        add_custom_command(
                TARGET ${target_name}
                POST_BUILD
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                COMMAND python ${CMAKE_SOURCE_DIR}/markdown_docu_builder.py ${DOXYGEN_XML_DIR} ${DOXYGEN_OUTPUT_FILE}
                COMMENT "Generating Markdown API documentation for ${target_name}"
        )
    else()
        message(WARNING "DOXYGEN not found! Skipping documentation generation for ${target_name}")
    endif ()
endfunction()

function(add_library_tests library_name test_dir)
    if(ENABLE_TESTS)
        find_package(Catch2 3 REQUIRED)

        file(GLOB_RECURSE TEST_SOURCES "${test_dir}/*.cpp")
        if(TEST_SOURCES)
            set(test_target "${library_name}Tests")
            add_executable(${test_target} ${TEST_SOURCES})

            target_link_libraries(${test_target} PRIVATE ${library_name} Catch2::Catch2WithMain)
            add_test(NAME ${test_target} COMMAND ${test_target})

            message(STATUS "Added tests for ${libary_name} from ${test_dir}")
        else ()
            message(WARNING "No tests found in ${test_dir}")
        endif()
    endif ()
endfunction()