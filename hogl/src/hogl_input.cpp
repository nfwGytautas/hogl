#include "hogl_input.hpp"

#include <iostream>
#include <algorithm>

#include <gl/glad.h>
#include <gl/glfw3.h>

#include "hogl_context.hpp"
#include "hogl_wnd.hpp"

HOGL_NSPACE_BEGIN

/**
 * @brief The input system that invoked glfwPollEvents() nullptr means that the user most probably
 * called it and even though that is not prohibited it will print an error since hogl the only way
 * that raw callbacks are hit if there are bound hogl windows. Mixing raw and hogl libraries is ok
 * but it should be either the entire system or not
*/
static hogl_cs_input* bound_system = nullptr;

#define VALIDATE_INTEGRITY if (bound_system == nullptr) { std::cout << "[hogl:warn] Nullptr bound system detected, don't mix hogl system functions with raw library functions, choose one\n"; return; }

void raw_mkey_callback(GLFWwindow* window, int button, int action, int mods)
{
	VALIDATE_INTEGRITY

	// Get hogl window from GLFW window
	hogl_wnd* wnd = (hogl_wnd*)glfwGetWindowUserPointer(window);

	// Construct event
	hogl_e_mkey e = { button, (hogl_e_action)action, (hogl_e_modifier)mods, wnd };
	bound_system->send_event(e);
}

void raw_mmove_callback(GLFWwindow* window, double xpos, double ypos)
{
	VALIDATE_INTEGRITY

	// Get hogl window from GLFW window
	hogl_wnd* wnd = (hogl_wnd*)glfwGetWindowUserPointer(window);

	// Construct event
	hogl_e_mmove e = { xpos, ypos, wnd };
	bound_system->send_event(e);
}

void raw_mscroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	VALIDATE_INTEGRITY

	// Get hogl window from GLFW window
	hogl_wnd* wnd = (hogl_wnd*)glfwGetWindowUserPointer(window);

	// Construct event
	hogl_e_mscroll e = { xoffset, yoffset, wnd };
	bound_system->send_event(e);
}

void raw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	VALIDATE_INTEGRITY

	// Get hogl window from GLFW window
	hogl_wnd* wnd = (hogl_wnd*)glfwGetWindowUserPointer(window);

	// Construct event
	hogl_e_key e = { key, (hogl_e_action)action, (hogl_e_modifier)mods, wnd };
	bound_system->send_event(e);
}

hogl_cs_input::~hogl_cs_input()
{
	for (hogl_e_interface* iface : m_interfaces)
	{
		this->destroy_interface(iface);
	}
}

void hogl_cs_input::update()
{
	bool needs_cleanup = false;

	for (hogl_e_interface* iface : m_interfaces)
	{
		if (iface->destroy)
		{
			this->destroy_interface(iface);
		}
	}

	// Remove destroyed windows
	if (needs_cleanup)
	{
		m_interfaces.erase(std::remove(std::begin(m_interfaces), std::end(m_interfaces), nullptr), std::end(m_interfaces));
	}

	// Poll events, this will cause callbacks to be called
	bound_system = this;
	glfwPollEvents();
	bound_system = nullptr;

	// Flush all events
	this->flush();
}

void hogl_cs_input::flush()
{
	size_t size = std::max({ m_q_mkey.size(), m_q_mmove.size(), m_q_mscroll.size(), m_q_key.size() });

	for (size_t i = 0; i < size; i++)
	{
		hogl_e_mkey* mkey = i < m_q_mkey.size() ? &m_q_mkey[i] : nullptr;
		hogl_e_mmove* mmove = i < m_q_mmove.size() ? &m_q_mmove[i] : nullptr;
		hogl_e_mscroll* mscroll = i < m_q_mscroll.size() ? &m_q_mscroll[i] : nullptr;
		hogl_e_key* key = i < m_q_key.size() ? &m_q_key[i] : nullptr;

		// Call callbacks
		for (hogl_e_interface* iface : m_interfaces)
		{
			if (!iface->active)
			{
				continue;
			}

			if (iface->cb_mkey && mkey)
			{
				iface->cb_mkey(iface->user_pointer, mkey);
			}

			if (iface->cb_mmove && mmove)
			{
				iface->cb_mmove(iface->user_pointer, mmove);
			}

			if (iface->cb_mscroll && mscroll)
			{
				iface->cb_mscroll(iface->user_pointer, mscroll);
			}

			if (iface->cb_key && key)
			{
				iface->cb_key(iface->user_pointer, key);
			}
		}
	}

	m_q_mkey.clear();
	m_q_mmove.clear();
	m_q_mscroll.clear();
	m_q_key.clear();
}

void hogl_cs_input::bind_wnd(hogl_wnd* wnd)
{
	GLFWwindow* glWindow = (GLFWwindow*)wnd->native_window;

	// Set callbacks
	glfwSetMouseButtonCallback(glWindow, raw_mkey_callback);
	glfwSetCursorPosCallback(glWindow, raw_mmove_callback);
	glfwSetScrollCallback(glWindow, raw_mscroll_callback);
	glfwSetKeyCallback(glWindow, raw_key_callback);
}

hogl_e_interface* hogl_cs_input::new_interface()
{
	hogl_e_interface* interface = new hogl_e_interface();
	interface->active = 1;
	interface->destroy = 0;
	m_interfaces.push_back(interface);
	return interface;
}

void hogl_cs_input::send_event(const hogl_e_mkey& e)
{
	m_q_mkey.push_back(e);
}

void hogl_cs_input::send_event(const hogl_e_mmove& e)
{
	m_q_mmove.push_back(e);
}

void hogl_cs_input::send_event(const hogl_e_mscroll& e)
{
	m_q_mscroll.push_back(e);
}

void hogl_cs_input::send_event(const hogl_e_key& e)
{
	m_q_key.push_back(e);
}

void hogl_cs_input::destroy_interface(hogl_e_interface*& iface)
{
	delete iface;
	iface = nullptr;
}

HOGL_NSPACE_END
