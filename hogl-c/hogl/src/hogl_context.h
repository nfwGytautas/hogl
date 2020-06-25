#pragma once

// Common hogl library types
#include "hogl_types.h"

// Define used to simplify development by configuring that the C/C++ side can be compiled with any option
// and that bindgen will still be able to use this file to create bindings for Rust
HOGL_C_BLOCK_START


/**
 * @brief hogl context structure contains all the needed information for all hogl operations
 * it is used to make sure the library is initialized, keeps track of event loops etc.
*/
struct hogl_context {
	/**
	 * @brief Implementation details for the hogl context
	*/
	hogl_context_impl* details;
};

/**
 * @brief Initializes hogl library, this function should only be called once, calling twice is undefined
 * behavior
 * @return hogl_context instance
*/
hogl_context* hogl_init();

/**
 * @brief Shuts down the hogl library, this function should only be called once, calling twice is undefined
 * behavior
 * @param context hogl context recieved from hogl_init function
*/
void hogl_shutdown(hogl_context** context);


HOGL_C_BLOCK_END
