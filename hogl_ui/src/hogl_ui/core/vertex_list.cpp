#include "vertex_list.hpp"

#include <algorithm>
#include "hogl_core/graphics/hogl_render.h"

constexpr auto VERTICE_VBO_ID = 0;
constexpr auto INDICE_VBO_ID = 1;

constexpr auto VERTICE_COUNT = 8;

namespace hogl_ui {
	vertex_list::vertex_list() {
		hogl_vao_new(&m_vao);

		// Create the buffer, doesn't matter about the initial size, just the attributes
		hogl_ap_desc ap_desc[3];

		// Vertices
		ap_desc[0].divisor = 0;
		ap_desc[0].ecount = 3;
		ap_desc[0].index = 0;
		ap_desc[0].normalized = false;
		ap_desc[0].offset = 0;
		ap_desc[0].type = HOGL_ET_FLOAT;

		// UV
		ap_desc[1].divisor = 0;
		ap_desc[1].ecount = 2;
		ap_desc[1].index = 1;
		ap_desc[1].normalized = false;
		ap_desc[1].offset = 3 * sizeof(float);
		ap_desc[1].type = HOGL_ET_FLOAT;

		// Color
		ap_desc[2].divisor = 0;
		ap_desc[2].ecount = 3;
		ap_desc[2].index = 2;
		ap_desc[2].normalized = false;
		ap_desc[2].offset = 5 * sizeof(float);
		ap_desc[2].type = HOGL_ET_FLOAT;

		hogl_vbo_desc desc[2];
		desc[0].usage = HOGL_VBOU_DYNAMIC;
		desc[0].type = HOGL_VBOT_ARRAY_BUFFER;
		desc[0].data = nullptr;
		desc[0].data_size = 0;
		desc[0].ap_desc = &ap_desc[0];
		desc[0].desc_size = 3;
		desc[0].stride = VERTICE_COUNT * sizeof(float);

		desc[1].usage = HOGL_VBOU_DYNAMIC;
		desc[1].type = HOGL_VBOT_ELEMENT_BUFFER;
		desc[1].data = nullptr;
		desc[1].data_size = 0;

		// Allocate buffer
		hogl_vbo_new(&m_vbo, desc[0]);
		hogl_vbo_new(&m_ebo, desc[1]);

		hogl_vao_bind(m_vao);
		hogl_vbo_bind(m_vbo);
		hogl_vbo_bind(m_ebo);
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
		size_t s = m_vertices.size() / VERTICE_COUNT;
		
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
			if (hogl_vbo_data(m_vbo, m_vertices.data(), m_vertices.size() * sizeof(float), 0) != HOGL_ERROR_NONE) {
				return;
			}
		
			if (hogl_vbo_data(m_ebo, m_indices.data(), m_indices.size() * sizeof(unsigned int), 0) != HOGL_ERROR_NONE) {
				return;
			}
		
			m_lfsize = m_vertices.size();
		}
		else {
			// TODO: Optimize so that only changed regions get mapped
		
			// We need to resize the vbo
			if (hogl_vbo_data(m_vbo, m_vertices.data(), m_vertices.size() * sizeof(float), 0) != HOGL_ERROR_NONE) {
				return;
			}

			if (hogl_vbo_data(m_ebo, m_indices.data(), m_indices.size() * sizeof(unsigned int), 0) != HOGL_ERROR_NONE) {
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
