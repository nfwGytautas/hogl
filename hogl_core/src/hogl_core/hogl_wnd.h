/**
* @brief hogl window file contains functionality and data necessary to work with windows
*/

#ifndef _HOGL_WINDOW_
#define _HOGL_WINDOW_

#include "hogl_core.h"
#include <stdbool.h>

/**
 * @brief hogl window represents a single rendering window, however this is by itself
 * a OpenGL context due to the way GLFW windows work. So this is both an OpenGL context
 * and a window
*/
typedef struct _hogl_wnd hogl_wnd;

/**
 * @brief Callback for window closing, if the function returns false then the window closing
 * will be aborted otherwise it will be continued and the window interface is_open will be set to false
*/
typedef bool(*hogl_wi_close_cb)(void*);

/**
 * @brief hogl window interface is a struct that contains user space data like if the windows is closed,
 * event callbacks that can be set, the struct is auto managed by the window and once the window is destroyed so is
 * its interface
*/
typedef struct _hogl_wi {
	/**
	 * @brief User pointer that will be passed to all callbacks
	*/
	void* usrp;

	/**
	 * @brief If the window associated with this interface is open then this variable will evaluate to true
	*/
	bool is_open;

	// Callbacks
	hogl_wi_close_cb close_cb;
} hogl_wi;

/**
 * @brief Creates a new window and stores the data inside 'p' variable,
 * the window will later need to be destroyed using hogl_destroy_window, the context associated with the
 * window will automatically be made current of the thread
 * 
 * @param p The pointer where the newly created window will be stored or NULL if there was an error
 * @return Returns error codes:
 *		HOGL_ERROR_NONE			if window creation was successful
 *		HOGL_ERROR_WND_CREATE	if window creation failed
 *		HOGL_ERROR_GLAD_INIT	if GLAD failed to load OpenGL functions
*/
HOGL_API hogl_error hogl_new_window(hogl_wnd** p);

/**
 * @brief Activates the OpenGL context associated with the specified window on current thread
 * @param window hogl_window that will be activated
*/
HOGL_API void hogl_activate_context(hogl_wnd* window);

/**
 * @brief Internally calls glfwSwapBuffers on the specified window
 * @param window Window to swap buffers for
*/
HOGL_API void hogl_swap_window_buffer(hogl_wnd* window);

/**
 * @brief Internally calls glfwPollEvents
*/
HOGL_API void hogl_update_window_states(void);

/**
 * @brief Returns the window interface instance associated with the window
 * @param window Window whose interface is required
 * @return Pointer to the interface
*/
HOGL_API hogl_wi* hogl_get_wi_ptr(hogl_wnd* window);

/**
 * @brief Destroys the specified window
 * @param window window to destroy
*/
HOGL_API void hogl_destroy_window(hogl_wnd* window);

#endif