
/**
* hogl render file contains the rendering commands needed to render primitives
*/

#ifndef _HOGL_RENDER_
#define _HOGL_RENDER_

#include <stdbool.h>
#include "hogl_core/shared/hogl_def.h"

/**
 * @brief A render state struct is used to hold the rendering state and is primarily a
 * convenience structure
*/
typedef struct _hogl_rstate {
	hogl_render_depth depth;
} hogl_rstate;

/**
 * @brief Clears the color of the active window buffer with the specified color
 * @param r Red color channel value
 * @param g Green color channel value
 * @param b Blue color channel value
 * @param a Alpha color channel value
*/
HOGL_API void hogl_render_clear(float r, float g, float b, float a);

/**
 * @brief Sets the new viewport, does not change the window size
 * @param width Width of the new viewport
 * @param height Height of the new viewport
*/
HOGL_API void hogl_viewport(unsigned int width, unsigned int height);

/**
 * @brief Enables/Disables seamless cubemap rendering
 * @param enabled True for enabled, false otherwise
*/
HOGL_API void hogl_seamless_cm(bool enabled);

/**
 * @brief Returns the default render state preset
 * @return hogl_rstate with default values
*/
HOGL_API hogl_rstate hogl_default_rstate(void);

/**
 * @brief Sets the hogl render state to the one inside the rstate parameter
 * @param rstate hogl_rstate structure contaning the new rendering state
*/
HOGL_API void hogl_set_rstate(hogl_rstate rstate);

/**
 * @brief Enables/Disables rendering depth state check and sets its function comparison function
 * @param depth hogl_render_depth enum value
*/
HOGL_API void hogl_set_depth_test(hogl_render_depth depth);

/**
 * @brief Renders the currently bound vao without using a index buffer
 * @param mode Mode to interpret data as
 * @param vertices Number of vertices inside the vao
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 *		HOGL_ERROR_UNKNOWN_MODE		if the specified render mode is invalid
*/
HOGL_API hogl_error hogl_render_a(hogl_render_mode mode, unsigned int vertices);

/**
 * @brief Renders the currently bound vao using a index buffer
 * @param mode Mode to interpret data as
 * @param vertices Number of indices inside the ebo
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 *		HOGL_ERROR_UNKNOWN_MODE		if the specified render mode is invalid
*/
HOGL_API hogl_error hogl_render_e(hogl_render_mode mode, unsigned int vertices);

/**
 * @brief Instanced version of hogl_render_a
 * @param mode Mode to interpret data as
 * @param vertices Number of vertices inside the vao
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 *		HOGL_ERROR_UNKNOWN_MODE		if the specified render mode is invalid
*/
HOGL_API hogl_error hogl_render_ai(hogl_render_mode mode, unsigned int vertices, unsigned int instances);

/**
 * @brief Instanced version of hogl_render_e
 * @param mode Mode to interpret data as
 * @param vertices Number of vertices inside the vao
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 *		HOGL_ERROR_UNKNOWN_MODE		if the specified render mode is invalid
*/
HOGL_API hogl_error hogl_render_ei(hogl_render_mode mode, unsigned int vertices, unsigned int instances);

#endif