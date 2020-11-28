#include "hogl_core.h"

#include "hogl_core/hogl_log.h"

#include <gl/glad.h>
#include <gl/glfw3.h>

static bool sb_glfwInitialized = false;

void glfw_log_cb(int errCode, const char* msg) {
	hogl_log_error("GLFW error: [%l] %s", errCode, msg);
}

const char* hogl_error_desc(hogl_error err)
{
	switch (err) {

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

	sb_glfwInitialized = true;
	hogl_log_info("hogl initialized");
	return HOGL_ERROR_NONE;
}

void hogl_shutdown(void)
{
	// Check if all memory is freed, hogl will free all data it ever allocated if done correctly 
	// so this error will mean there was a memory leak, however if memory tracking is disabled
	// the allocations will be 0
	if (hogl_get_allocations() > 0) {
		hogl_log_error("Memory leak detected, there are still %d allocations, totaling to %d allocated bytes", hogl_get_allocations(), hogl_get_allocated_bytes());
	}
}
