macro(add_benchmark_executable libname source_files)
    set(_source_files ${source_files} ${ARGN})
    set(target_name "${libname}_benchmarks")
    set(modified_source_files)
    set(benchmarks_directory "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks")

    foreach(source_file ${_source_files})
        list(APPEND modified_source_files "${benchmarks_directory}/${source_file}")
    endforeach()

    message("Adding benchmark executable ${target_name} with source files: ${_source_files}")
    add_executable(${target_name} ${modified_source_files})

    target_link_libraries(${target_name} PRIVATE benchmark::benchmark_main ${libname})
    target_compile_definitions(${target_name} PUBLIC "BENCHMARKING=1")
    target_compile_options(${target_name} PRIVATE ${WARNING_FLAGS})
    set_target_properties(${target_name} PROPERTIES OUTPUT_NAME "benchmarks")
endmacro()
