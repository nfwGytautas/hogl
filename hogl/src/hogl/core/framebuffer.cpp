#include "framebuffer.hpp"

namespace hogl {

	framebuffer::framebuffer(hogl_framebuffer_desc desc) {
		hogl_framebuffer_new(&m_fbo, desc);
	}

	framebuffer::~framebuffer() {
		hogl_framebuffer_free(m_fbo);
	}

	void framebuffer::bind() {
		hogl_framebuffer_bind(m_fbo);
	}

	void framebuffer::ca(hogl::relay_ptr<hogl::texture> texture, int slot, int mip) {
		hogl_framebuffer_ca(m_fbo, texture->m_texture, slot, mip);
	}

}
