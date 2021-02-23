#include "window.hpp"

namespace hogl {
	window::window()
	{
		// Create new window
		if (hogl_new_window(&m_hwindow) != HOGL_ERROR_NONE) {
			// Failed to create a window
			return;
		}
	}

	window::~window() {
		hogl_destroy_window(m_hwindow);
	}

	void window::swap_buffer() {
		hogl_swap_window_buffer(m_hwindow);
	}

	hogl_wi* window::get_wi() {
		return hogl_get_wi_ptr(m_hwindow);
	}
}
