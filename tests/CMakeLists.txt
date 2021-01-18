#========================================
# tests
#========================================

# Source files
file(GLOB_RECURSE TEST_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

add_executable(tests ${TEST_SOURCE_FILES})

set_property(TARGET tests PROPERTY CXX_STANDARD 17)
set_property(TARGET tests PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Includes
target_include_directories(tests PUBLIC ${PROJECT_SOURCE_DIR}/hogl_core/src/)
target_include_directories(tests PUBLIC ${PROJECT_SOURCE_DIR}/hogl_ui/src/)
target_link_libraries(tests PRIVATE hogl_core PRIVATE hogl_ui)

# Some MSVC flags
set_property(TARGET tests PROPERTY
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")