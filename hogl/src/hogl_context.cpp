#include "hogl_context.hpp"

#include <iostream>

#include <gl/glad.h>
#include <gl/glfw3.h>

#include "hogl_input.hpp"
#include "hogl_wnd.hpp"
#include "hogl_renderer.hpp"
#include "hogl_loader.hpp"

HOGL_NSPACE_BEGIN

hogl_context* hogl_init()
{
	hogl_context* pContext = nullptr;

	std::cout << "[hogl:info] Initializing hogl context\n";

	std::cout << "[hogl:info] GLFW\n";

	// Set error callback
	glfwSetErrorCallback(
		[](int errCode, const char* msg)
	{
		std::cout << "[hogl:error] GLFW error:\n Code: " << errCode << "\n Message: " << msg << "\n";
	});

	if (glfwInit() != GLFW_TRUE)
	{
		return pContext;
	};

	// Since we use glDebugMessageCallback we need to specify OpenGL version to be atleast 4.3
#ifdef _DEBUG
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
#endif
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	pContext = new hogl_context();
	pContext->window_manager = new hogl_cs_wndmanager();
	pContext->input_system = new hogl_cs_input();
	pContext->renderer = new hogl_cs_renderer();
	pContext->loader = new hogl_cs_loader();

	std::cout << "[hogl:info] hogl initialized\n";

	return pContext;
}

void hogl_shutdown(hogl_context*& context)
{
	std::cout << "[hogl:info] hogl shutting down\n";

	// Make sure we don't double free
	if (context != nullptr)
	{
		delete context->input_system;
		delete context->window_manager;
		delete context->renderer;
		delete context->loader;

		delete context;
		context = nullptr;

		glfwTerminate();
	}
	else
	{
		std::cout << "[hogl:warn] Trying to double free hogl_context\n";
	}
}

HOGL_NSPACE_END
