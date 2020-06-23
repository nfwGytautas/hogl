cmake_minimum_required(VERSION 3.16.2 FATAL_ERROR)

# Project
project(hogl)

# Compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/bin)

# Export all library symbols
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-all-symbols")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)

# Includes
include_directories(${CMAKE_SOURCE_DIR}/src/)
include_directories(${CMAKE_SOURCE_DIR}/lib/)

# Lib folders
link_directories(${CMAKE_SOURCE_DIR}/lib/)

# Defines
add_compile_definitions(HOGL_VS)

# Executable
file(GLOB SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/src/*.h)
add_library(hogl STATIC ${SOURCE_FILES})

# Some MSVC flags
set_property(TARGET hogl PROPERTY
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")


set_property(TARGET hogl PROPERTY 
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src/")

# Libraries to link
target_link_libraries(hogl PUBLIC Glad)
target_link_libraries(hogl PUBLIC glfw3)

# Copy output
set(BuildDir ${CMAKE_SOURCE_DIR}/../../build/)

# Gather list of all .xml and .conf files in "/config"
file(GLOB LibFiles ${CMAKE_SOURCE_DIR}/lib/*.lib)

foreach(LibFile ${LibFiles})
  add_custom_command(TARGET hogl PRE_BUILD
                     COMMAND ${CMAKE_COMMAND} -E
                         copy ${LibFile} ${BuildDir})
endforeach()

# Copy hogl API
#add_custom_command(TARGET hogl POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:hogl> ${BuildDir}
#    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$<CONFIG>/hogl.lib ${BuildDir})

add_custom_command(TARGET hogl POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:hogl> ${BuildDir})
