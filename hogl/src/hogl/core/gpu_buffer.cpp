#include "gpu_buffer.hpp"

namespace hogl {

	gpu_buffer::gpu_buffer(const hogl_vbo_desc& desc) 
		: m_type(desc.type), m_stride(desc.stride), m_len(desc.data_size)
	{
		hogl_vbo_new(&m_vbo, desc);
	}

	gpu_buffer::~gpu_buffer() {
		hogl_vbo_free(m_vbo);
	}

	size_t gpu_buffer::count() {
		return m_len / m_stride;
	}

	hogl_vbo_type gpu_buffer::type() {
		return m_type;
	}

	void gpu_buffer::bind() {
		hogl_vbo_bind(m_vbo);
	}

	void gpu_buffer::set_data_generic(void* data, size_t len, size_t offset) {
		m_len = len;
		hogl_vbo_data(m_vbo, data, len, offset);
	}

}
