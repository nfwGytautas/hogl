#include "hogl_context.hpp"

#include <iostream>

#include <gl/glad.h>
#include <gl/glfw3.h>

#include "hogl_input.hpp"
#include "hogl_wnd.hpp"

HOGL_NSPACE_BEGIN

hogl_context* hogl_init()
{
	hogl_context* pContext = new hogl_context();
	pContext->window_manager = new hogl_cs_wndmanager();
	pContext->input_system = new hogl_cs_input();

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

void hogl_shutdown(hogl_context*& context)
{
	std::cout << "[hogl:info] hogl shutting down\n";

	// Make sure we don't double free
	if (context != nullptr)
	{
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