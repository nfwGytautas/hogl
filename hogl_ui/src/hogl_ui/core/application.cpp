#include "application.hpp"

#include <fstream>
#include <sstream>

#include "hogl_core/hogl_core.h"
#include "hogl_ui/core/ui_parser.hpp"

#define CLEAR_COLOR 1.0f, 0.0f, 0.5f, 1.0f

void* operator new(size_t size) {
	return hogl_malloc(size);
}

void operator delete(void* p) {
	hogl_free(p);
}

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

		// Create window
		if (hogl_new_window(&m_window) != HOGL_ERROR_NONE) {
			// Error in creating the window
			hogl_log_error("Failed to create a window");
			return;
		}

		// Default viewport
		hogl_viewport(1280, 720);
		hogl_set_depth_test(HOGL_RD_LEQUAL);

		// Query the window interface
		m_wi = hogl_get_wi_ptr(m_window);

		// Create vertex list
		m_vlist.quad = new vertex_list();

		// Load shaders
		setup_vlists();

		// Root element
		m_root = new root();
		tinfo& t = m_root->get_transform();
		t.actual_width = this->get_window_width();
		t.actual_height = this->get_window_height();

		hogl_log_trace("Application created");

		gs_application = this;
	}

	application::~application() {
		hogl_log_trace("Shutting down the application");
		delete m_root;
		delete m_vlist.quad;
		hogl_destroy_window(m_window);
		hogl_shutdown();
	}

	bool application::load_layout_file(const char* path){
		hogl_log_trace("Loading layout: %s", path);
		ui_parser parser(m_root);
		return parser.parse(path);
	}

	void application::show() {
		// Setup initial transform
		m_root->transform();

		while (m_wi->is_open) {
			// Clear
			hogl_render_clear(CLEAR_COLOR);

			// Generate the vertex list
			m_root->queue_vertices(m_vlist);

			// Render the vertex list
			m_vlist.quad->render();

			// Render the elements
			m_root->render();

			// Swap buffers and update states
			hogl_swap_window_buffer(m_window);
			hogl_update_window_states();
		}
	}

	application* application::get_instance() {
		return gs_application;
	}

	unsigned int application::get_window_height() const {
		return 720;
	}

	unsigned int application::get_window_width() const {
		return 1280;
	}

	void application::setup_vlists() {
		std::ifstream input_file;
		std::stringstream buffer;

		std::string vertex_source;
		std::string fragment_source;

		buffer.str("");
		input_file.open("res/ui/quad.vs");
		buffer << input_file.rdbuf();
		vertex_source = buffer.str();
		input_file.close();

		buffer.str("");
		input_file.open("res/ui/quad.fs");
		buffer << input_file.rdbuf();
		fragment_source = buffer.str();
		input_file.close();

		m_vlist.quad->load_shader(vertex_source, fragment_source);
	}
}
