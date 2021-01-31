#include "shader.hpp"

namespace hogl {

	shader::shader(hogl_shader_desc desc)
	{
		hogl_shader_new(&m_shader, desc);
	}

	shader::~shader() {
		hogl_shader_free(m_shader);
	}

	void shader::sampler_location(const std::string& name, int bp) {
		hogl_shader_sampler_location(m_shader, name.c_str(), bp);
	}

	void shader::ubo_location(const std::string& name, int bp) {
		hogl_shader_ubo_binding(m_shader, name.c_str(), bp);
	}

	void shader::bind() {
		hogl_shader_bind(m_shader);
	}

}
