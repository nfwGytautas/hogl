#pragma once

// Common hogl library types
#include "hogl_types.h"

// Define used to simplify development by configuring that the C/C++ side can be compiled with any option
// and that bindgen will still be able to use this file to create bindings for Rust
HOGL_C_BLOCK_START


/**
 * @brief hogl window
*/
struct hogl_wnd {

	/**
	 * @brief hogl context that was used to create this window
	*/
	hogl_context* context;

	/**
	 * @brief [STATE] Specifies if this window is closed, the value is updated after calling hogl_wnd_state_update function
	*/
	int is_closed;

	/**
	 * @brief Native window of created with the help of GLFW
	 * in reality this is a type erased GLFW window
	*/
	void* native_window;
};

/**
 * @brief Creates a hogl window
 * @param context hogl context received from hogl_init function
 * @return hogl_wnd instance or null if there was an error
*/
hogl_wnd* hogl_wnd_create(hogl_context* context);

/**
 * @brief Destroys hogl_wnd
 * @param wnd Window received from hogl_wnd_create function to destroy
*/
void hogl_wnd_destroy(hogl_wnd** wnd);

/**
 * @brief Update window state variables for more info look at hogl_wnd struct
 * @param wnd hogl window struct
*/
void hogl_wnd_state_update(hogl_wnd* wnd);


HOGL_C_BLOCK_END
