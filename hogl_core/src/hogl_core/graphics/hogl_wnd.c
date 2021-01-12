#include "hogl_wnd.h"

#include <gl/glad.h>
#include <gl/glfw3.h>

#include "hogl_core/shared/hogl_log.h"
#include "hogl_core/shared/hogl_memory.h"

typedef struct _hogl_wnd {
	GLFWwindow* window;
	hogl_wi interface;
} hogl_wnd;

void GLAPIENTRY gl_error_cb(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userPointer) {
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		hogl_log_error("%s", message)
			break;

	case GL_DEBUG_TYPE_OTHER:
	case GL_DEBUG_TYPE_PERFORMANCE:
	case GL_DEBUG_TYPE_PORTABILITY:
		hogl_log_trace("%s", message);
		break;
	}
}

void wnd_close_cb(GLFWwindow* window) {
	hogl_wi* wi = &((hogl_wnd*)glfwGetWindowUserPointer(window))->interface;

	if (wi->close_cb != NULL) {
		if (!wi->close_cb(wi->usrp)) {
			glfwSetWindowShouldClose(window, GLFW_FALSE);
			wi->is_open = true;
			return;
		}
	}

	wi->is_open = false;
}

void map_callbacks(GLFWwindow* window) {
	glfwSetWindowCloseCallback(window, wnd_close_cb);
}

hogl_error hogl_new_window(hogl_wnd** p) {
	hogl_log_info("Creating a new window");

	*p = (hogl_wnd*)hogl_malloc((sizeof(hogl_wnd)));

	(*p)->window = glfwCreateWindow(1280, 720, "hogl", NULL, NULL);

	if ((*p)->window == NULL) {
		hogl_log_error("Failed to create a glfw window");
		return HOGL_ERROR_WND_CREATE;
	}

	// Activate context
	hogl_activate_context(*p);

	// glad load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		hogl_log_error("Failed to load OpenGL symbols");
		return HOGL_ERROR_GLAD_INIT;
	}

	// OpenGL error handling
#ifndef HOGL_SUPPRESS_GL_LOG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(gl_error_cb, NULL);

	// Disables buffer mapping messages
#ifndef HOGL_ENALBE_ALL_GL_LOGS
	glDebugMessageControl(33350, 33361, 33387, 0, NULL, GL_FALSE);
#endif

#endif

	// Join GLFW window together with hogl window
	glfwSetWindowUserPointer((*p)->window, *p);

	// Initialize interface
	hogl_wi* wi = &((*p)->interface);
	wi->usrp = NULL;
	wi->is_open = true;
	wi->close_cb = NULL;

	// Map the glfw window callbacks
	map_callbacks((*p)->window);

	return HOGL_ERROR_NONE;
}

void hogl_activate_context(hogl_wnd* window) {
	hogl_log_info("Activating a context");
	glfwMakeContextCurrent(window->window);
}

void hogl_swap_window_buffer(hogl_wnd* window) {
	glfwSwapBuffers(window->window);
}

void hogl_update_window_states(void) {
	glfwPollEvents();
}

hogl_wi* hogl_get_wi_ptr(hogl_wnd* window) {
	return &window->interface;
}

void hogl_destroy_window(hogl_wnd* window) {
	hogl_log_info("Destroying a window");
	glfwDestroyWindow(window->window);
	hogl_free(window);
}
