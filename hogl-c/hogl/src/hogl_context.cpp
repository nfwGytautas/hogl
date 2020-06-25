#include "hogl_context.h"

#include <iostream>

#include <gl/glad.h>
#include <gl/glfw3.h>

#include "hogl_input.h"
#include "hogl_cpp_types.h"

hogl_context* hogl_init()
{
	hogl_context* pContext = new hogl_context();
	pContext->details = new hogl_context_impl();

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
		// Clear registered interfaces
		for (unsigned int i = 0; i < (*context)->details->event_interfaces.size(); i++)
		{
			delete (*context)->details->event_interfaces[i];
		}

		glfwTerminate();

		delete (*context)->details;
		delete* context;
		*context = nullptr;
	}
	else
	{
		std::cout << "[hogl:warn] Trying to double free hogl_context\n";
	}
}
