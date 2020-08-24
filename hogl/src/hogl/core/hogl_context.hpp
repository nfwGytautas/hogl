#pragma once

#include <vector>

// Common hogl library types
#include "hogl/hogl_types.hpp"

HOGL_NSPACE_BEGIN

/**
 * @brief hogl context structure contains all the needed information for all hogl operations
 * it is used to make sure the library is initialized, it is made up of multiple systems that
 * are setup during hogl_init function. These systems can than be used individually where ever needed,
 * for more info consult each system class
*/
struct HOGL_API hogl_context {
	/**
	 * @brief Input system used for hogl
	*/
	hogl_cs_input* input_system;

	/**
	 * @brief Window manager system used for hogl
	*/
	hogl_cs_wndmanager* window_manager;

	/**
	 * @brief Rendering system used by hogl to provide common rendering abstractions
	*/
	hogl_cs_renderer* renderer;

	/**
	 * @brief External asset loading system used by hogl to provide mesh, audio, texture loading
	*/
	hogl_cs_loader* loader;

	/**
	 * @brief Handle to an instance of the logging configuration, updating this will update every system
	*/
	hogl_log_config* log_handle;
};

/**
 * @brief hogl library logging configuration
*/
struct HOGL_API hogl_log_config
{
	hogl_gl_log_level gl_log = hogl_gl_log_level::ALL;
};

/**
 * @brief Initializes hogl library, this function should only be called once, calling twice is undefined
 * behavior
 * @return hogl_context instance
*/
HOGL_API hogl_context* hogl_init();

/**
 * @brief Returns the current time in seconds, used to calculate frame delta time
 * @return GLFW seconds
*/
HOGL_API float hogl_system_clock();

/**
 * @brief Shuts down the hogl library, this function should only be called once, calling twice is undefined
 * behavior
 * @param context hogl context recieved from hogl_init function
*/
HOGL_API void hogl_shutdown(hogl_context*& context);

HOGL_NSPACE_END
