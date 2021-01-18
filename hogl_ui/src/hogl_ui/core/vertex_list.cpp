#include "vertex_list.hpp"

#include <algorithm>
#include "hogl_core/graphics/hogl_render.h"

constexpr auto VERTICE_VBO_ID = 0;
constexpr auto INDICE_VBO_ID = 1;

namespace hogl_ui {
	vertex_list::vertex_list() {
		hogl_vao_new(&m_vao);

		// Create the buffer, doesn't matter about the initial size, just the attributes
		hogl_ap_desc ap_desc[2];

		// Vertices
		ap_desc[0].divisor = 0;
		ap_desc[0].ecount = 3;
		ap_desc[0].index = 0;
		ap_desc[0].normalized = false;
		ap_desc[0].offset = 0;
		ap_desc[0].stride = 5 * sizeof(float);
		ap_desc[0].type = HOGL_ET_FLOAT;

		// UV
		ap_desc[1].divisor = 0;
		ap_desc[1].ecount = 2;
		ap_desc[1].index = 1;
		ap_desc[1].normalized = false;
		ap_desc[1].offset = 3 * sizeof(float);
		ap_desc[1].stride = 5 * sizeof(float);
		ap_desc[1].type = HOGL_ET_FLOAT;

		hogl_vbo_desc desc[2];
		desc[0].usage = HOGL_VBOU_DYNAMIC;
		desc[0].type = HOGL_VBOT_ARRAY_BUFFER;
		desc[0].data = nullptr;
		desc[0].data_size = 0;
		desc[0].ap_desc = &ap_desc[0];
		desc[0].desc_size = 2;

		desc[1].usage = HOGL_VBOU_DYNAMIC;
		desc[1].type = HOGL_VBOT_ELEMENT_BUFFER;
		desc[1].data = nullptr;
		desc[1].data_size = 0;

		// Allocate buffer
		hogl_vao_alloc_buffers(m_vao, desc, 2);
	}

	vertex_list::~vertex_list() {
		hogl_vao_free(m_vao);

		if (m_shader != nullptr) {
			hogl_shader_free(m_shader);
		}
	}

	void vertex_list::load_shader(const std::string& vertex, const std::string& fragment) {
		// Free existing shader if it exists
		if (m_shader != nullptr) {
			hogl_shader_free(m_shader);
			m_shader = nullptr;
		}

		hogl_shader_desc desc;
		desc.vertex_source = vertex.c_str();
		desc.fragment_source = fragment.c_str();

		hogl_shader_new(&m_shader, desc);
	}

	void vertex_list::add_range(const std::vector<float>& data, const std::vector<unsigned int>& indices) {
		size_t s = m_vertices.size() / 5;
		
		m_vertices.insert(m_vertices.end(), data.begin(), data.end());
		for (const unsigned int& i : indices) {
			m_indices.push_back(i + s);
		}
	}

	void vertex_list::render() {
		// If no vertices in the vertex list skip rendering
		if (m_vertices.size() == 0) {
			return;
		}
		
		if (m_lfsize < m_vertices.size()) {
			// We need to resize the vbo
			if (hogl_vao_buffer_resize(m_vao, VERTICE_VBO_ID, m_vertices.size() * sizeof(float), m_vertices.data()) != HOGL_ERROR_NONE) {
				return;
			}
		
			if (hogl_vao_buffer_resize(m_vao, INDICE_VBO_ID, m_indices.size() * sizeof(unsigned int), m_indices.data()) != HOGL_ERROR_NONE) {
				return;
			}
		
			m_lfsize = m_vertices.size();
		}
		else {
			// TODO: Optimize so that only changed regions get mapped
		
			// Map new data
			if (hogl_vao_buffer_data(m_vao, VERTICE_VBO_ID, 0, m_vertices.data(), m_vertices.size() * sizeof(float)) != HOGL_ERROR_NONE) {
				return;
			}
		
			if (hogl_vao_buffer_data(m_vao, INDICE_VBO_ID, 0, m_indices.data(), m_indices.size() * sizeof(unsigned int)) != HOGL_ERROR_NONE) {
				return;
			}
		}

		hogl_vao_bind(m_vao);
		hogl_shader_bind(m_shader);
		hogl_render_e(HOGL_RM_TRIANGLES, m_indices.size());

		// Clear the vertex list
		m_vertices.clear();
		m_indices.clear();
	}
}
