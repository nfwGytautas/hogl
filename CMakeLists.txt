cmake_minimum_required(VERSION 3.16.2 FATAL_ERROR)

project(hogl)

# Init
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)

set(DEVELOPMENT TRUE)

# Suite type
add_compile_definitions(HOGL_SUITE_FULL)

# Export all library symbols
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-all-symbols")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)

add_subdirectory(hogl_core)
add_subdirectory(hogl_ui)
add_subdirectory(hogl)
add_subdirectory(tests)
