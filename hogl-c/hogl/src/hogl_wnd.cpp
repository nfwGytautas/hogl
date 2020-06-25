#include "hogl_wnd.h"

#include <iostream>

#include <gl/glad.h>
#include <gl/glfw3.h>

#include "hogl_input.h"

hogl_wnd* hogl_wnd_create(hogl_context* context)
{
	std::cout << "[hogl:info] Creating a hogl window\n";

	// Create window and set it as current OpenGL context
	GLFWwindow* window = glfwCreateWindow(1280, 920, "hogl", NULL, NULL);
	glfwMakeContextCurrent(window);

	if (window == NULL)
	{
		std::cout << "[hogl:error] Failed to create GLFW window\n";
		return nullptr;
	}

	// glad load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "[hogl:error] Failed to initialize GLAD\n";
		return nullptr;
	}

	// Create hogl window
	hogl_wnd* pWnd = new hogl_wnd();

	pWnd->native_window = window;
	pWnd->context = context;
	pWnd->is_closed = false;

	// Join GLFW window together with hogl window
	glfwSetWindowUserPointer(window, pWnd);

	// Bind the window to input API
	hogl_input_bind_window(pWnd);

	return pWnd;
}

void hogl_wnd_destroy(hogl_wnd** wnd)
{
	std::cout << "[hogl:info] Destroying window\n";

	// Make sure we don't double free
	if (*wnd != nullptr)
	{
		// Destroy native window
		glfwDestroyWindow((GLFWwindow*)(*wnd)->native_window);

		delete* wnd;
		*wnd = nullptr;
	}
	else
	{
		std::cout << "[hogl:warn] Trying to double free hogl_wnd\n";
	}
}

void hogl_wnd_state_update(hogl_wnd* wnd)
{
	// Query window state
	wnd->is_closed = glfwWindowShouldClose((GLFWwindow*)wnd->native_window);
}
