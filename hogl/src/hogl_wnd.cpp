#include "hogl_wnd.hpp"

#include <iostream>

#include <gl/glad.h>
#include <gl/glfw3.h>

#include "hogl_input.hpp"

HOGL_NSPACE_BEGIN

hogl_cs_wndmanager::~hogl_cs_wndmanager()
{
	for (hogl_wnd* wnd : m_windows)
	{
		this->destroy_wnd(wnd);
	}
}

void hogl_cs_wndmanager::update()
{
	bool needs_cleanup = false;

	for (hogl_wnd* wnd : m_windows)
	{
		if (wnd == nullptr)
		{
			needs_cleanup = true;
			continue;
		}

		if (wnd->destroy)
		{
			this->destroy_wnd(wnd);
			needs_cleanup = true;
			continue;
		}

		if (wnd->active)
		{
			// Updates the window
			this->update_wnd(wnd);
		}
	}

	// Remove destroyed windows
	if (needs_cleanup)
	{
		m_windows.erase(std::remove(std::begin(m_windows), std::end(m_windows), nullptr), std::end(m_windows));
	}
}

hogl_wnd* hogl_cs_wndmanager::create()
{
	hogl_wnd* pWnd;
	GLFWwindow* window;

	std::cout << "[hogl:info] Creating a hogl window\n";

	// Create window and set it as current OpenGL context
	window = glfwCreateWindow(1280, 920, "hogl", NULL, NULL);
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
	pWnd = new hogl_wnd();

	pWnd->native_window = window;
	pWnd->is_closed = false;
	pWnd->active = 1;
	pWnd->destroy = 0;

	// Join GLFW window together with hogl window
	glfwSetWindowUserPointer(window, pWnd);

	m_windows.push_back(pWnd);

	return pWnd;
}

void hogl_cs_wndmanager::update_wnd(hogl_wnd* wnd)
{
	// Query window state
	wnd->is_closed = glfwWindowShouldClose((GLFWwindow*)wnd->native_window);
}

void hogl_cs_wndmanager::destroy_wnd(hogl_wnd*& wnd)
{
	std::cout << "[hogl:info] Destroying window\n";

	// Destroy native window
	glfwDestroyWindow((GLFWwindow*)wnd->native_window);

	delete wnd;
	wnd = nullptr;
}

HOGL_NSPACE_END
