#pragma once

#include <vector>

// Common hogl library types
#include "hogl_types.hpp"

HOGL_NSPACE_BEGIN

/**
 * @brief hogl input system, used to take care of the end user
*/
class HOGL_API hogl_cs_input
{
public:
	~hogl_cs_input();

	/**
	 * @brief Update all active input interfaces, also destroys interfaces marked for that
	*/
	void update();

	/**
	 * @brief Flushes events, make sure to call this after manually using send_event functions
	*/
	void flush();

	/**
	 * @brief Binds the window to the raw input API
	 * @param wnd Windows to bind
	*/
	void bind_wnd(hogl_wnd* wnd);

	/**
	 * @brief Create new input interface
	 * @return New interface instance
	*/
	hogl_e_interface* new_interface();

	/**
	 * @brief Sends event to the system, allows to simulate user input, while still using the same system everywhere
	 * @param e Event to send
	*/
	void send_event(const hogl_e_mkey& e);

	/**
	 * @brief Sends event to the system, allows to simulate user input, while still using the same system everywhere
	 * @param e Event to send
	*/
	void send_event(const hogl_e_mmove& e);

	/**
	 * @brief Sends event to the system, allows to simulate user input, while still using the same system everywhere
	 * @param e Event to send
	*/
	void send_event(const hogl_e_mscroll& e);

	/**
	 * @brief Sends event to the system, allows to simulate user input, while still using the same system everywhere
	 * @param e Event to send
	*/
	void send_event(const hogl_e_key& e);
private:
	/**
	 * @brief Destroy specified interface
	 * @param iface Interface to destroy
	*/
	void destroy_interface(hogl_e_interface*& iface);
private:
	std::vector<hogl_e_interface*> m_interfaces;

	std::vector<hogl_e_mkey> m_q_mkey;
	std::vector<hogl_e_mmove> m_q_mmove;
	std::vector<hogl_e_mscroll> m_q_mscroll;
	std::vector<hogl_e_key> m_q_key;
};

/**
 * @brief hogl mouse button event
*/
struct HOGL_API hogl_e_mkey
{
	/**
	 * @brief Button that caused the event
	*/
	int button;

	/**
	 * @brief Event action
	*/
	hogl_e_action action;

	/**
	 * @brief Modifier for this event for more check hogl_e_modifier
	*/
	hogl_e_modifier modifier;

	/**
	 * @brief Window where the event originated from
	*/
	hogl_wnd* window;
};

/**
 * @brief hogl mouse moved event
*/
struct HOGL_API hogl_e_mmove
{
	/**
	 * @brief Mouse position in the x axis, relative to left edge
	*/
	double x;

	/**
	 * @brief MOuse position in the y axis, relative to top edge
	*/
	double y;

	/**
	 * @brief Window where the event originated from
	*/
	hogl_wnd* window;
};

/**
 * @brief hogl mouse scroll event
*/
struct HOGL_API hogl_e_mscroll
{
	/**
	 * @brief X axis offset
	*/
	double x;

	/**
	 * @brief Y axis offset
	*/
	double y;

	/**
	 * @brief Window where the event originated from
	*/
	hogl_wnd* window;
};

/**
 * @brief hogl keyboard key input event
*/
struct HOGL_API hogl_e_key
{
	/**
	 * @brief Button that caused the event
	*/
	int key;

	/**
	 * @brief Event action
	*/
	hogl_e_action action;

	/**
	 * @brief Modifier for this event for more check hogl_e_modifier
	*/
	hogl_e_modifier modifier;

	/**
	 * @brief Window where the event originated from
	*/
	hogl_wnd* window;
};

/**
 * @brief hogl event bus is used to create an event sink for hogl events
*/
struct HOGL_API hogl_e_interface
{
	/**
	 * @brief User pointer for this interface
	*/
	void* user_pointer;

	/**
	 * @brief Specifies if this interface should invoked or not, 0 for NO otherwise YES, default YES
	*/
	int active;

	/**
	 * @brief Specifies if this interface should be destroyed or not, 0 for NO otherwise YES, default NO
	*/
	int destroy;

	hogl_ecb_mkey cb_mkey;
	hogl_ecb_mmove cb_mmove;
	hogl_ecb_mscroll cb_mscroll;
	hogl_ecb_key cb_key;
};

HOGL_NSPACE_END
