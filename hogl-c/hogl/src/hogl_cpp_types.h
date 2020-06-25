#pragma once

// This file is where cpp definitions are stored for hogl types mainly implementations

#include <vector>

/**
 * @brief hogl context implementation
*/
struct _hogl_context_impl
{
	/**
	 * @brief Registered interfaces for the hogl context
	*/
	std::vector<hogl_e_interface*> event_interfaces;
};