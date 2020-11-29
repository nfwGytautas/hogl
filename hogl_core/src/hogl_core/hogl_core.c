#include "hogl_core.h"

#include "hogl_core/hogl_log.h"

#include <gl/glad.h>
#include <gl/glfw3.h>

void glfw_log_cb(int errCode, const char* msg) {
	hogl_log_error("GLFW error: [%d] %s", errCode, msg);
}

const char* hogl_error_desc(hogl_error err)
{
	switch (err) {
		case HOGL_ERROR_GLFW_INIT:
			return "GLFW could not be initialized";
		case HOGL_ERROR_WND_CREATE:
			return "A window could not be created";
		case HOGL_ERROR_GLAD_INIT:
			return "GLAD failed to initialize or load OpenGL symbols";
	default:
		return NULL;
	}
}

hogl_error hogl_init(void)
{
	hogl_log_trace("Initializing hogl");

	glfwSetErrorCallback(glfw_log_cb);

	if (glfwInit() == GLFW_FALSE) {
		hogl_log_error("Failed to initialize GLFW!");
		return HOGL_ERROR_GLFW_INIT;
	}

	// Only need 4 if logging
#ifndef HOGL_SUPPRESS_GL_LOG
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
#endif
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	hogl_log_info("hogl initialized");
	return HOGL_ERROR_NONE;
}

void hogl_shutdown(void)
{
	hogl_log_trace("Shutting down hogl");

	// Check if all memory is freed, hogl will free all data it ever allocated if done correctly 
	// so this error will mean there was a memory leak, however if memory tracking is disabled
	// the allocations will be 0
	if (hogl_get_allocations() > 0) {
		hogl_log_error("Memory leak detected, there are still %d allocations, totaling to %d allocated bytes", hogl_get_allocations(), hogl_get_allocated_bytes());
	}
}
