#pragma once

#include "hogl/shared/hogl_def.hpp"
#include "hogl_core/graphics/hogl_gl_primitive.h"

namespace hogl {

	/**
	 * @brief A mesh object provided by hogl, this abstracts a VAO object, tho the actual VAO
	 * will be different due to instance VBO being bound
	*/
	class HOGL_CPP_API mesh {
	public:
		mesh();
		~mesh();

		/**
		 * @brief Binds the mesh
		*/
		void bind();
	private:
		hogl_vao* m_vao = nullptr;
	};

}
