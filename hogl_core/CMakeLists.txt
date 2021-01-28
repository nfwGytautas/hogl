#========================================
# hogl core
#========================================

file(GLOB_RECURSE CORE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

# Lib mode
add_library(hogl_core SHARED ${CORE_SOURCE_FILES})
target_compile_definitions(hogl_core PUBLIC HOGL_EXPORT)

# Lib folders
target_link_directories(hogl_core PUBLIC ${PROJECT_SOURCE_DIR}/dependencies/)

# Includes
target_include_directories(hogl_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)
target_include_directories(hogl_core PUBLIC ${PROJECT_SOURCE_DIR}/dependencies/)

add_custom_command(TARGET hogl_core POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/dependencies/OpenAL32.dll"
        "${CMAKE_BINARY_DIR}/bin")

#set_property(TARGET hogl_core PROPERTY C_STANDARD 11)
set_property(TARGET hogl_core PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Libraries to link
target_link_libraries(hogl_core PRIVATE Glad PRIVATE glfw3 PRIVATE OpenAL32)

# Some MSVC flags
set_property(TARGET hogl_core PROPERTY
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")