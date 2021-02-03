#pragma once

#include "hogl/shared/hogl_def.hpp"
#include "hogl/core/object.hpp"

#include "hogl_core/graphics/hogl_gl_primitive.h"

namespace hogl {

	/**
	 * @brief Uniform buffer object abstraction, designed to be inherited for each UBO
	*/
	class HOGL_CPP_API ubo {
	public:
		/**
		 * @brief Creates a new uniform buffer object
		 * @param glsl_name Name of the UBO in glsl shaders (needs to be unique)
		 * @param bp Universal bind point (needs to be unique)
		 * @param size Size of the UBO
		*/
		ubo(const std::string& glsl_name, unsigned int bp, size_t size);
		virtual ~ubo();

		/**
		 * @brief Updates GPU side memory with the current contents of the inner buffer
		*/
		void update();
	protected:
		void* p_iBuff = nullptr;
	private:
		hogl_ubo* m_ubo = nullptr;
		std::string m_glslName = "";
		unsigned int m_bp = 0;
		size_t m_size = 0;

		friend class shader;
	};

}
