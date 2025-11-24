add_library_module(common)

target_precompile_headers(common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/pch.hpp)