#========================================
# hogl core
#========================================

file(GLOB_RECURSE CPP_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

# Lib mode
add_library(hogl SHARED ${CPP_SOURCE_FILES})
target_compile_definitions(hogl PUBLIC HOGL_CPP_EXPORT PUBLIC ENTT_DLL_EXPORT)

# Lib folders
target_link_directories(hogl PUBLIC ${PROJECT_SOURCE_DIR}/dependencies/)

# Includes
target_include_directories(hogl PUBLIC ${PROJECT_SOURCE_DIR}/hogl_core/src/)
target_include_directories(hogl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)
target_include_directories(hogl PUBLIC ${PROJECT_SOURCE_DIR}/dependencies/)

set_property(TARGET hogl PROPERTY CXX_STANDARD 17)
set_property(TARGET hogl PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Libraries to link
target_link_libraries(hogl PRIVATE hogl_core)

# Some MSVC flags
set_property(TARGET hogl PROPERTY
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")