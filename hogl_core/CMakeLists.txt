cmake_minimum_required(VERSION 3.16.2 FATAL_ERROR)

project(hogl_core C)



# Init
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)

set(DEVELOPMENT TRUE)



# Export all library symbols
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-all-symbols")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)

# Includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/)

# Lib folders
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/)

# Source files
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

# Suite type
add_compile_definitions(HOGL_SUITE_FULL)

# Defines
if (${DEVELOPMENT})

# Executable mode
add_compile_definitions(HOGL_DEVELOPMENT)
add_executable(hogl ${SOURCE_FILES})

else()

# Lib mode
add_compile_definitions(HOGL_EXPORT)
add_library(hogl SHARED ${SOURCE_FILES})

endif()

add_custom_command(TARGET hogl POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/OpenAL32.dll"
        "${CMAKE_BINARY_DIR}/bin")

#set_property(TARGET hogl PROPERTY C_STANDARD 11)
set_property(TARGET hogl PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Libraries to link
target_link_libraries(hogl PRIVATE Glad PRIVATE glfw3 PRIVATE OpenAL32)

# Some MSVC flags
set_property(TARGET hogl PROPERTY
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
