cmake_minimum_required(VERSION 3.28.0)
project("Ultra low latency L2/L3 orderbook")

include(tools/cmake/executable_macros.cmake)
include(tools/cmake/lib_macros.cmake)
include(tools/cmake/required_packages.cmake)
include(tools/cmake/test_macros.cmake)
include(tools/cmake/benchmark_macros.cmake)
include(tools/cmake/macros.cmake)

add_option(ENABLE_SANITIZERS "Enables fsan sanitizers")
add_option(ENABLE_STATIC_ANALYSIS "Enables clang-tidy static analysis")

logged_set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -march=native")
logged_set(CMAKE_CXX_STANDARD 23)
logged_set(CMAKE_CXX_STANDARD_REQUIRED ON)
logged_set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
logged_set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
logged_set(ASAN_FLAGS -fsanitize=address,leak,undefined,float-divide-by-zero,float-cast-overflow)
logged_set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}")
logged_set(
    CLANG_WARNINGS
    -Wall
    -Wextra
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2
    -Wimplicit-fallthrough
    -Werror
)

add_subdirectory(modules)

if(ENABLE_STATIC_ANALYSIS)
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy" REQUIRED)
endif()

if(ENABLE_SANITIZERS)
    add_link_options(-fsanitize=address,undefined,float-divide-by-zero,float-cast-overflow)
    add_compile_options(-fsanitize=address,undefined,float-divide-by-zero,float-cast-overflow -fno-omit-frame-pointer)
endif()