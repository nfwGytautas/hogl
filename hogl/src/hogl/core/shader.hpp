#pragma once

#include <string>
#include "hogl/shared/hogl_def.hpp"
#include "hogl/shared/pointer.hpp"
#include "hogl/core/object.hpp"
#include "hogl/core/ubo.hpp"

#include "hogl_core/graphics/hogl_gl_primitive.h"

namespace hogl {

	/**
	 * @brief Thin wrapper around hogl C API shader
	*/
	class HOGL_CPP_API shader : public object {
		HOGL_OBJECT(0);
	public:
		shader(hogl_shader_desc desc);
		~shader();

		/**
		 * @brief Sets the sampler location of the shader
		 * @param name Name of the sampler variable
		 * @param bp Bind point for the sampler
		*/
		void sampler_location(const std::string& name, int bp);

		/**
		 * @brief Attaches the specified ubo to this shader
		 * @param ubo UBO to attach
		*/
		void ubo_attach(relay_ptr<ubo> ubo);

		/**
		 * @brief Binds the shader
		*/
		void bind();
	private:
		hogl_shader* m_shader = nullptr;
	};

}
