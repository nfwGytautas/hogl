#include "application.hpp"

#include "hogl_core/hogl_core.h"

namespace hogl_ui {
	static application* gs_application = nullptr;

	application::application() {
		hogl_log_trace("Creating new ui application");

		// Check if we are not trying to create a second application
		if (gs_application != nullptr) {
			hogl_log_error("Trying to initialize a second ui application");
			return;
		}

		// Init hogl
		if (hogl_init() != HOGL_ERROR_NONE) {
			// Error in initialization
			hogl_log_error("Failed to initialize hogl");
			return;
		}

		hogl_log_trace("Application created");

		gs_application = this;
	}

	application::~application() {
		hogl_shutdown();
	}

	bool application::load_layout_file(const char* path){
		return false;
	}

	void application::show() {
		
	}

	application* application::get_instance() {
		return gs_application;
	}
}
