/**
* @brief hogl window file contains functionality and data necessary to work with windows
*/

#ifndef _HOGL_WINDOW_
#define _HOGL_WINDOW_

#include "hogl_core.h"

/**
 * @brief hogl window represents a single rendering window, however this is by itself
 * a OpenGL context due to the way GLFW windows work. So this is both an OpenGL context
 * and a window
*/
typedef struct _hogl_wnd hogl_wnd;

/**
 * @brief Creates a new window and stores the data inside 'p' variable,
 * the window will later need to be destroyed using hogl_destroy_window
 * 
 * @param p The pointer where the newly created window will be stored or NULL if there was an error
 * @return Returns error codes:
 *		HOGL_ERROR_NONE			if window creation was successful
*/
hogl_error hogl_new_window(hogl_wnd** p);

/**
 * @brief Destroys the specified window
 * @param window window to destroy
*/
void hogl_destroy_window(hogl_wnd* window);

#endif