#include "button.hpp"

#include "hogl_ui/core/application.hpp"

namespace hogl_ui {
	button::button(element* parent, const std::string& name)
		: element(parent, name)
	{
		parent->add_child(this, "");

		// Test
		vertices = {
			// positions        // texture Coords
			 1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			-1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			-1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f
		};

		indices = {
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};
	}

	button::~button()
	{
	}

	void button::queue_vertices(const vertex_lists& vlist) {
		vlist.quad->add_range(vertices, indices);
	}

	void button::render() {

	}

	void button::transform() {
		// Update object transform
		tinfo& et = this->get_transform();
		tinfo& pt = this->get_parent()->get_transform();

		et.actual_width = pt.actual_width * et.rel_width;
		et.actual_height = pt.actual_height * et.rel_height;

		et.actual_x = pt.actual_x + pt.actual_width * (et.rel_x);
		et.actual_y = pt.actual_y + pt.actual_height * (et.rel_y);

		float xscale = et.actual_width / application::get_instance()->get_window_width();
		float yscale = et.actual_height / application::get_instance()->get_window_height();

		float xoffset = et.actual_x / application::get_instance()->get_window_width();
		float yoffset = et.actual_y / application::get_instance()->get_window_height();

		for (size_t i = 0; i < vertices.size() / 8; i++) {
			vertices[0 + i * 8] *= xscale;
			vertices[0 + i * 8] += xoffset;

			vertices[1 + i * 8] *= yscale;
			vertices[1 + i * 8] += yoffset;
		}
	}

	void button::set_background(color c) {
		for (size_t i = 0; i < vertices.size() / 8; i++) {
			vertices[5 + i * 8] = c.r / 255.0f;
			vertices[6 + i * 8] = c.g / 255.0f;
			vertices[7 + i * 8] = c.b / 255.0f;
		}
	}
}
