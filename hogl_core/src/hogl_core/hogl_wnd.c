#include "hogl_wnd.h"

#include <gl/glad.h>
#include <gl/glfw3.h>

#include "hogl_core/hogl_log.h"
#include "hogl_core/hogl_memory.h"

static bool glfwInitialized = false;

typedef struct _hogl_wnd {
	GLFWwindow* window;
} hogl_wnd;

hogl_error hogl_new_window(hogl_wnd** p)
{
	*p = (hogl_wnd*)hogl_malloc((sizeof(hogl_wnd)));
	return HOGL_ERROR_NONE;
}

void hogl_destroy_window(hogl_wnd* window)
{
	hogl_free(window);
}
