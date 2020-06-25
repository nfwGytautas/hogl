#include "hogl_input.h"

#include "hogl_context.h"
#include "hogl_wnd.h"
#include "hogl_cpp_types.h"

#include <gl/glad.h>
#include <gl/glfw3.h>

void raw_mkey_callback(GLFWwindow* window, int button, int action, int mods)
{
	// Get hogl window from GLFW window
	hogl_wnd* wnd = (hogl_wnd*)glfwGetWindowUserPointer(window);

	// Construct event
	hogl_e_mkey e = { button, (hogl_e_action)action, (hogl_e_modifier)mods, wnd };

	// Call context callbacks
	for (hogl_e_interface* interface : wnd->context->details->event_interfaces)
	{
		if (interface->active && interface->cb_key)
		{
			interface->cb_mkey(interface->user_pointer, &e);
		}
	}
}

void raw_mmove_callback(GLFWwindow* window, double xpos, double ypos)
{
	// Get hogl window from GLFW window
	hogl_wnd* wnd = (hogl_wnd*)glfwGetWindowUserPointer(window);

	// Construct event
	hogl_e_mmove e = { xpos, ypos, wnd };

	// Call context callbacks
	for (hogl_e_interface* interface : wnd->context->details->event_interfaces)
	{
		if (interface->active && interface->cb_mmove)
		{
			interface->cb_mmove(interface->user_pointer, &e);
		}
	}
}

void raw_mscroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	// Get hogl window from GLFW window
	hogl_wnd* wnd = (hogl_wnd*)glfwGetWindowUserPointer(window);

	// Construct event
	hogl_e_mscroll e = { xoffset, yoffset, wnd };

	// Call context callbacks
	for (hogl_e_interface* interface : wnd->context->details->event_interfaces)
	{
		if (interface->active && interface->cb_mscroll)
		{
			interface->cb_mscroll(interface->user_pointer, &e);
		}
	}
}

void raw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Get hogl window from GLFW window
	hogl_wnd* wnd = (hogl_wnd*)glfwGetWindowUserPointer(window);

	// Construct event
	hogl_e_key e = { key, (hogl_e_action)action, (hogl_e_modifier)mods, wnd };

	// Call context callbacks
	for (hogl_e_interface* interface : wnd->context->details->event_interfaces)
	{
		if (interface->active && interface->cb_key)
		{
			interface->cb_key(interface->user_pointer, &e);
		}
	}
}

void hogl_input_bind_window(hogl_wnd* window)
{
	GLFWwindow* glWindow = (GLFWwindow*)window->native_window;

	// Set callbacks
	glfwSetMouseButtonCallback(glWindow, raw_mkey_callback);
	glfwSetCursorPosCallback(glWindow, raw_mmove_callback);
	glfwSetScrollCallback(glWindow, raw_mscroll_callback);
	glfwSetKeyCallback(glWindow, raw_key_callback);
}

void hogl_input_update(hogl_context* context)
{
	if (context != nullptr) {
		glfwPollEvents();
	}
}

hogl_e_interface* hogl_input_new_interface(hogl_context* context)
{
	hogl_e_interface* interface = new hogl_e_interface();
	interface->active = 1;
	context->details->event_interfaces.push_back(interface);
	return interface;
}
