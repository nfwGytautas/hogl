#include "texture.hpp"

namespace hogl {
	texture::texture() {}

	texture::texture(hogl_texture_desc desc) {
		hogl_texture_new(&m_texture, desc);
	}

	texture::~texture() {
		hogl_texture_free(m_texture);
	}

	void texture::set_data(hogl_texture_data& data) {
		hogl_set_texture_data(m_texture, &data);
	}

	void texture::gen_mipmap() {
		hogl_texture_gen_mipmap(m_texture);
	}

	void texture::bind(unsigned int slot) {
		hogl_texture_bind(m_texture, slot);
	}

	cubemap::cubemap(hogl_texture_desc desc) {
		hogl_cm_new(&m_texture, desc);
	}

	cubemap::~cubemap() {
	}

	void cubemap::set_side(hogl_cm_side side) {
		hogl_cm_active_side(m_texture, side);
	}

}
