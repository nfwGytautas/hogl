#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <sstream>

#include "hogl_core/hogl_core.h"
#include "pbr_cpp.h"
//#include "pbr.h"
#include "ui.h"
#include "lib.h"

void pbr(void) {
	hogl_wnd* hwindow = NULL;
	hogl_wi* hwi = NULL;

	// Initialize hogl
	if (hogl_init() != HOGL_ERROR_NONE) {
		// Failed to initialize hogl
		return;
	}

	// Create new window
	if (hogl_new_window(&hwindow) != HOGL_ERROR_NONE) {
		// Failed to create a window
		return;
	}

	// Not necessary but just configures to the default state
	hogl_set_rstate(hogl_default_rstate());

	// Get window interface
	hwi = hogl_get_wi_ptr(hwindow);

	// Prepare for pbr rendering
	prepare_pbr();

	while (hwi->is_open) {
		render_pbr();

		// Swap buffer and check for events
		hogl_swap_window_buffer(hwindow);
		hogl_update_window_states();
	}

	pbr_free();

	hogl_destroy_window(hwindow);

	hogl_shutdown();
}

int main(int argc, char** argv) {
	//ui();
	lib();
	pbr();

	return 0;
}
