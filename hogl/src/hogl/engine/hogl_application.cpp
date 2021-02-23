#include "hogl_application.hpp"

#include "hogl_core/hogl_core.h"

namespace hogl {
	hogl_application* hogl_application::ms_instance = nullptr;

	hogl_application::hogl_application() {
		ms_instance = this;
	}

	hogl_application::~hogl_application() {
		delete ostorage;
		delete ubo_manager;
		delete asset_manager;
		delete instance_data;
		delete main_window;

		// Shutdown hogl
		hogl_shutdown();
	}

	hogl_application* hogl_application::get() {
		return ms_instance;
	}

	void hogl_application::init() {
		// Initialize hogl
		if (hogl_init() != HOGL_ERROR_NONE) {
			// Failed to initialize hogl
			return;
		}

		// Not necessary but just configures to the default state
		hogl_set_rstate(hogl_default_rstate());

		main_window = new window();

		ostorage = new hogl::object_storage();
		ubo_manager = new hogl::ubo_manager();
		asset_manager = new hogl::asset_manager();
		instance_data = new hogl::instance_data();

		hogl_wi* hwi = main_window->get_wi();

		on_ready();

		while (hwi->is_open) {
			on_update();
			on_render();

			// Swap buffer and check for events
			main_window->swap_buffer();

			// TODO: Transfer to input manager
			hogl_update_window_states();
		}
	}

	void hogl_application::on_ready() {

	}

	void hogl_application::on_update() {

	}

	void hogl_application::on_render() {

	}

	void hogl_application::loop() {

	}
}
