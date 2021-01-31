#include "mesh.hpp"

#include "hogl_core/graphics/hogl_render.h"

namespace hogl {
	mesh::mesh() {
		hogl_vao_new(&m_vao);
	}

	mesh::~mesh() {
		hogl_vao_free(m_vao);
	}

	void mesh::add_buffer(const ref<gpu_buffer>& buffer) {
		if (buffer->type() == HOGL_VBOT_ELEMENT_BUFFER) {
			m_ecount = buffer->count();
		}
		else {
			m_vcount = buffer->count();
		}

		this->bind();
		buffer->bind();

		m_buffers.push_back(buffer);
	}

	void mesh::bind() {
		hogl_vao_bind(m_vao);
	}

	void mesh::render(hogl_render_mode mode) {
		if (m_ecount > 0) {
			hogl_render_e(mode, m_ecount);
		}
		else {
			hogl_render_a(mode, m_vcount);
		}
	}

}
