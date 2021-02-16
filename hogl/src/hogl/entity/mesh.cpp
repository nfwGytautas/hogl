#include "mesh.hpp"

#include "hogl_core/graphics/hogl_render.h"

namespace hogl {
	mesh::mesh() {
		hogl_vao_new(&m_vao);
	}

	mesh::~mesh() {
		hogl_vao_free(m_vao);
	}

	void mesh::set_render_mode(hogl_render_mode rmode) {
		m_rmode = rmode;
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

	void mesh::render() {
		if (m_ecount > 0) {
			hogl_render_e(m_rmode, m_ecount);
		}
		else {
			hogl_render_a(m_rmode, m_vcount);
		}
	}

	void mesh::render_instanced(size_t count) {
		if (m_ecount > 0) {
			hogl_render_ei(m_rmode, m_ecount, count);
		}
		else {
			hogl_render_ai(m_rmode, m_vcount, count);
		}
	}

}
