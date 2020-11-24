#include "hogl_core.h"

#include <gl/glad.h>
#include <gl/glfw3.h>

static bool sb_glfwInitialized = false;

const char* hogl_error_desc(hogl_error err)
{
	switch (err) {

	default:
		return NULL;
	}
}

hogl_error hogl_init()
{
	if (glfwInit() == GLFW_FALSE) {

	}

	return HOGL_ERROR_NONE;
}
