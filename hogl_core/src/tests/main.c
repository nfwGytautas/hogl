#include <stdio.h>

#include "hogl_core/hogl_core.h"

#include <gl/glad.h>
#include <gl/glfw3.h>

void errorFn(const char* msg) {

}

int main(int argc, char** argv) {
	hogl_log_error("error");
	hogl_log_warn("warning");
	hogl_log_info("info");
	hogl_log_trace("trace");

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

	return 0;
}
