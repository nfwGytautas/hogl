#include "hogl.h"

#include <iostream>

#include <gl/glad.h>
#include <gl/glfw3.h>

hogl_context* hogl_init()
{
	hogl_context* pContext = new hogl_context();

	std::cout << "[hogl:info] Initializing hogl context\n";

	std::cout << "[hogl:info] GLFW\n";

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	std::cout << "[hogl:info] hogl initialized\n";

	return pContext;
}

void hogl_shutdown(hogl_context** context)
{
	std::cout << "[hogl:info] hogl shutting down\n";

	// Make sure we don't double free
	if (*context != nullptr)
	{
		glfwTerminate();

		delete *context;
		*context = nullptr;
	}
	else
	{
		std::cout << "[hogl:warn] Trying to double free hogl_context\n";
	}
}

hogl_wnd* hogl_wnd_create(hogl_context* context)
{
	// Create hogl window
	hogl_wnd* pWnd = new hogl_wnd();

	std::cout << "[hogl:info] Creating a hogl window\n";

	// Create window and set it as current OpenGL context
	GLFWwindow* window = glfwCreateWindow(1280, 920, "hogl", NULL, NULL);
	glfwMakeContextCurrent(window);

	if (window == NULL)
	{
		std::cout << "[hogl:error] Failed to create GLFW window\n";
		return nullptr;
	}
	pWnd->mp_native_window = window;

	//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetCursorPosCallback(window, mouse_callback);
	//glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "[hogl:error] Failed to initialize GLAD\n";
		return nullptr;
	}

	return pWnd;
}

void hogl_wnd_destroy(hogl_wnd** wnd)
{
	std::cout << "[hogl:info] Destroying window\n";

	// Make sure we don't double free
	if (*wnd != nullptr) 
	{
		// Destroy native window
		glfwDestroyWindow((GLFWwindow*)(*wnd)->mp_native_window);

		delete *wnd;
		*wnd = nullptr;
	}
	else 
	{
		std::cout << "[hogl:warn] Trying to double free hogl_wnd\n";
	}
}
