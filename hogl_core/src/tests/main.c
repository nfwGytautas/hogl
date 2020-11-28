#include <stdio.h>

#include "hogl_core/hogl_core.h"

#include <gl/glad.h>
#include <gl/glfw3.h>

void errorFn(const char* msg) {

}

int main(int argc, char** argv) {
	hogl_wnd* hwindow = NULL;

	// Initialize hogl
	if (hogl_init() != HOGL_ERROR_NONE) {
		// Failed to initialize hogl
		return 1;
	};

	// Create new window
	if (hogl_new_window(&hwindow) != HOGL_ERROR_NONE) {
		// Failed to create a window
		return 2;
	}

	printf("Hello world!\n");

	glfwInit();

	GLFWwindow* window = glfwCreateWindow(1280, 720, "hogl", NULL, NULL);
	glfwMakeContextCurrent(window);

	if (window == NULL)
	{
		//std::cout << "[hogl:error] Failed to create GLFW window\n";
		//return nullptr;
	}

	// glad load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		//std::cout << "[hogl:error] Failed to initialize GLAD\n";
		//return nullptr;
	}

	// OpenGL error handling
#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	//glDebugMessageCallback(gl_error_cb, m_logConfig);
#endif

	// Join GLFW window together with hogl window
	//glfwSetWindowUserPointer(window, pWnd);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	hogl_destroy_window(hwindow);
	hogl_shutdown();

	return 0;
}
