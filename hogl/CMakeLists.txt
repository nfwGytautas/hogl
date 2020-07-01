# Export all library symbols
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-all-symbols")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)

# Includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/)

# Lib folders
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/)

# Defines
#add_compile_definitions(HOGL_VS)

# Executable
file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)
add_library(hogl SHARED ${SOURCE_FILES})

# Some MSVC flags
set_property(TARGET hogl PROPERTY
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set_property(TARGET hogl PROPERTY 
	VS_DEBUGGER_WORKING_DIRECTORY "${BuildDir}")

# Libraries to link
target_link_libraries(hogl PRIVATE Glad PRIVATE glfw3)

# Copy headers 
add_custom_command(TARGET hogl PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/ ${HeaderDir})

# DLL and Lib
add_custom_command(TARGET hogl POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:hogl> ${BuildDir}
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$<CONFIG>/hogl.lib ${BuildDir})
