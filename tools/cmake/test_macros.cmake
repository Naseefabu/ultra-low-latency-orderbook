macro(add_test_executable libname source_files)
    set(_source_files ${source_files} ${ARGN})
    set(target_name "${libname}_tests")
    set(modified_source_files)
    set(tests_directory "${CMAKE_CURRENT_SOURCE_DIR}/tests")

    foreach(source_file ${_source_files})
        list(APPEND modified_source_files "${tests_directory}/${source_file}")
    endforeach()

    message("Adding test executable ${target_name} with source files: ${_source_files}")
    add_executable(${target_name} ${modified_source_files})

    target_link_libraries(${target_name} PRIVATE GTest::gmock GTest::gtest_main ${libname})
    target_compile_definitions(${target_name} PUBLIC "TESTING=1")
    target_compile_options(${target_name} PRIVATE ${WARNING_FLAGS})
    set_target_properties(${target_name} PROPERTIES OUTPUT_NAME "tests")

    gtest_discover_tests(${target_name}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
        PROPERTIES ENVIRONMENT "MSAN_OPTIONS=disable_coredump=0:abort_on_error=1"
    )
endmacro()

macro(add_e2e_test_executable libname source_files)
    set(_source_files ${source_files} ${ARGN})
    set(target_name "${libname}_tests_e2e")
    set(modified_source_files)
    set(tests_directory "${CMAKE_CURRENT_SOURCE_DIR}/tests/e2e/")

    foreach(source_file ${_source_files})
        list(APPEND modified_source_files "${tests_directory}/${source_file}")
    endforeach()

    message("Adding e2e test executable ${target_name} with source files: ${_source_files}")
    add_executable(${target_name} ${modified_source_files})

    target_link_libraries(${target_name} PRIVATE ${libname})
    target_compile_definitions(${target_name} PUBLIC "TESTING=1")
    target_compile_options(${target_name} PRIVATE ${WARNING_FLAGS})
    set_target_properties(${target_name} PROPERTIES OUTPUT_NAME "tests_e2e")

    gtest_discover_tests(${target_name}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
        PROPERTIES ENVIRONMENT "MSAN_OPTIONS=disable_coredump=0:abort_on_error=1"
    )
endmacro()
