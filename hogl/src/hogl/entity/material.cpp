#include "material.hpp"

#include "hogl_core/shared/hogl_log.h"

namespace hogl {

	void material::set_texture(const ref<texture>& texture, size_t slot) {
		if (slot > MAX_TEXTURE_UNITS) {
			hogl_log_error("Trying to set texture to slot %ld which is more than the maximum of %ld", slot, MAX_TEXTURE_UNITS);
			return;
		}

		m_textures[slot] = texture;
	}

	void material::set_shader(const ref<shader>& shader) {
		m_shader = shader;
	}

	void material::bind() {
		// Shader
		m_shader->bind();

		// Textures
		for (size_t i = 0; i < m_textures.size(); i++) {
			if (m_textures[i].valid()) {
				m_textures[i]->bind(i);
			}
		}
	}

}
