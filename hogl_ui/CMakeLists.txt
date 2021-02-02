#========================================
# hogl ui
#========================================

file(GLOB_RECURSE UI_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

# Lib mode
add_library(hogl_ui SHARED ${UI_SOURCE_FILES})
target_compile_definitions(hogl_ui PRIVATE HOGL_UI_EXPORT)

# Lib folders
target_link_directories(hogl_ui PUBLIC ${PROJECT_SOURCE_DIR}/dependencies/)

# Includes
target_include_directories(hogl_ui PUBLIC ${PROJECT_SOURCE_DIR}/hogl_core/src/)
target_include_directories(hogl_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)
target_include_directories(hogl_ui PUBLIC ${PROJECT_SOURCE_DIR}/dependencies/)

set_property(TARGET hogl_ui PROPERTY CXX_STANDARD 17)
set_property(TARGET hogl_ui PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Libraries to link
target_link_libraries(hogl_ui PRIVATE hogl_core PRIVATE pugixml)

# Some MSVC flags
set_property(TARGET hogl_ui PROPERTY
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")