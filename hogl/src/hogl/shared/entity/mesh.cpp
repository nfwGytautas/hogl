#include "mesh.hpp"

namespace hogl {

	hogl::mesh::mesh() {
		hogl_vao_new(&m_vao);
	}

	mesh::~mesh() {
		hogl_vao_free(m_vao);
	}

	void mesh::bind() {
		hogl_vao_bind(m_vao);
	}

}
