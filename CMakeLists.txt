cmake_minimum_required(VERSION 3.16.2 FATAL_ERROR)

project(hogl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)


# Variables
set(BuildDir ${CMAKE_CURRENT_SOURCE_DIR}/build/)
set(ShaderDir ${BuildDir}shaders/)
set(ResourceDir ${BuildDir}res/)
set(HeaderDir ${BuildDir}hogl/)

# Create dir
file(MAKE_DIRECTORY ${HeaderDir})
file(MAKE_DIRECTORY ${ShaderDir})

# hogl
add_subdirectory(hogl)

# sandbox
add_subdirectory(sandbox)
