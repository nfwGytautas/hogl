#include "ubo.hpp"

#include "hogl_core/shared/hogl_log.h"
#include "hogl_core/shared/hogl_memory.h"

namespace hogl {
	ubo::ubo(const std::string& glsl_name, unsigned int bp, size_t size)
		: m_glslName(glsl_name), m_bp(bp), m_size(size)
	{
		hogl_ubo_desc desc;
		desc.bp = m_bp;
		desc.offset = 0;
		desc.stride = m_size;

		hogl_ubo_new(&m_ubo, desc);
	}

	ubo::~ubo() {
		if (m_ubo != nullptr) {
			hogl_ubo_free(m_ubo);
		}
	}

	void ubo::update() {
#ifndef HOGL_DISABLE_DEBUG
		if (p_iBuff == nullptr) {
			hogl_log_error("nullptr UBO update tried");
			return;
		}

		if (m_ubo == nullptr) {
			hogl_log_error("Non initialized UBO update tried");
			return;
		}
#endif

		hogl_ubo_data(m_ubo, p_iBuff, m_size);
	}

}
