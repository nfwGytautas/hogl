#include <stdio.h>

#include "hogl_core/hogl_core.h"

#include <gl/glad.h>
#include <gl/glfw3.h>

int main(int argc, char** argv) {
	hogl_wnd* hwindow = NULL;
	hogl_wi* hwi = NULL;

	// Initialize hogl
	if (hogl_init() != HOGL_ERROR_NONE) {
		// Failed to initialize hogl
		return 1;
	}

	// Create new window
	if (hogl_new_window(&hwindow) != HOGL_ERROR_NONE) {
		// Failed to create a window
		return 2;
	}

	// Get window interface
	hwi = hogl_get_wi_ptr(hwindow);

	while (hwi->is_open) {

		// Swap buffer and check for events
		hogl_swap_window_buffer(hwindow);
		hogl_update_window_states();
	}

	hogl_destroy_window(hwindow);
	hogl_shutdown();

	return 0;
}
