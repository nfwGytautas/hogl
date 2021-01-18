#include "button.hpp"

namespace hogl_ui {
	button::button(element* parent, const std::string& name)
		: element(parent, name)
	{
		parent->add_child(this, "");

		// Test
		vertices = {
			// positions        // texture Coords
			 0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 1.0f, 0.0f
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

	void button::transform(const tinfo& t) {
		for (size_t i = 0; i < vertices.size() / 5; i++) {
			vertices[0 + i * 5] *= t.x_scale;
			vertices[0 + i * 5] += t.x_offset;

			vertices[1 + i * 5] *= t.y_scale;
			vertices[1 + i * 5] += t.y_offset;

			vertices[2 + i * 5] *= t.z_scale;
			vertices[2 + i * 5] += t.z_offset;
		}
	}
}
