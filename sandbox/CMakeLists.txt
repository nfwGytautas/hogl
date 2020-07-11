# Includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/)
include_directories(${BuildDir})

# Lib folders
link_directories(${BuildDir})

# Defines
#add_compile_definitions(HOGL_VS)

# Executable
file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)
add_executable(sandbox ${SOURCE_FILES})

# Set hogl as dependency
add_dependencies(sandbox hogl)

# Some MSVC flags
set_property(TARGET sandbox PROPERTY
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

set_property(TARGET sandbox PROPERTY 
	VS_DEBUGGER_WORKING_DIRECTORY "${BuildDir}")

# Libraries to link
target_link_libraries(sandbox PUBLIC hogl)

add_custom_command(TARGET sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sandbox> ${BuildDir})

# Copy headers 
add_custom_command(TARGET sandbox PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/ ${ShaderDir})

# Copy headers 
add_custom_command(TARGET sandbox PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/res/ ${ResourceDir})
