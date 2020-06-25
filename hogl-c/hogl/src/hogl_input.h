#pragma once

// Common hogl library types
#include "hogl_types.h"

// Define used to simplify development by configuring that the C/C++ side can be compiled with any option
// and that bindgen will still be able to use this file to create bindings for Rust
HOGL_C_BLOCK_START


/**
 * @brief hogl mouse button event 
*/
struct hogl_e_mkey
{
	/**
	 * @brief Button that caused the event
	*/
	int button;

	/**
	 * @brief Event action
	*/
	hogl_e_action action;

	/**
	 * @brief Modifier for this event for more check hogl_e_modifier
	*/
	hogl_e_modifier modifier;

	/**
	 * @brief Window where the event originated from
	*/
	hogl_wnd* window;
};

/**
 * @brief hogl mouse moved event
*/
struct hogl_e_mmove 
{
	/**
	 * @brief Mouse position in the x axis, relative to left edge
	*/
	double x;

	/**
	 * @brief MOuse position in the y axis, relative to top edge
	*/
	double y;

	/**
	 * @brief Window where the event originated from
	*/
	hogl_wnd* window;
};

/**
 * @brief hogl mouse scroll event
*/
struct hogl_e_mscroll
{
	/**
	 * @brief X axis offset
	*/
	double x;

	/**
	 * @brief Y axis offset
	*/
	double y;

	/**
	 * @brief Window where the event originated from
	*/
	hogl_wnd* window;
};

/**
 * @brief hogl keyboard key input event
*/
struct hogl_e_key
{
	/**
	 * @brief Button that caused the event
	*/
	int key;

	/**
	 * @brief Event action
	*/
	hogl_e_action action;

	/**
	 * @brief Modifier for this event for more check hogl_e_modifier
	*/
	hogl_e_modifier modifier;

	/**
	 * @brief Window where the event originated from
	*/
	hogl_wnd* window;
};

/**
 * @brief hogl event bus is used to create an event sink for hogl events
*/
struct hogl_e_interface
{
	/**
	 * @brief User pointer for this interface
	*/
	void* user_pointer;

	/**
	 * @brief Integer for specifying if this event interface should receive events or not 1 for yes, 0 for no, default 1
	*/
	int active;

	hogl_ecb_mkey cb_mkey;
	hogl_ecb_mmove cb_mmove;
	hogl_ecb_mscroll cb_mscroll;
	hogl_ecb_key cb_key;
};

/**
 * @brief hogl internal function used to bind hogl window to native input API
 * @param context valid hogl window
*/
void hogl_input_bind_window(hogl_wnd* window);

/**
 * @brief Checks for input since the last time this was called
 * @param context hogl context
*/
void hogl_input_update(hogl_context* context);

/**
 * @brief Creates a new hogl event interface, interface are automatically freed upon hogl shutdown, by default the interface is active
 * @return hogl event interface pointer
*/
hogl_e_interface* hogl_input_new_interface(hogl_context* context);


HOGL_C_BLOCK_END
