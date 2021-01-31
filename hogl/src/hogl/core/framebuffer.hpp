#pragma once

#include "hogl/shared/hogl_def.hpp"
#include "hogl/shared/pointer.hpp"
#include "hogl/core/object.hpp"
#include "hogl/core/texture.hpp"
#include "hogl_core/graphics/hogl_gl_primitive.h"

namespace hogl {

	/**
	 * @brief Thin wrapper around hogl C API framebuffer
	*/
	class HOGL_CPP_API framebuffer : public object {
		HOGL_OBJECT(6);
	public:
		framebuffer(hogl_framebuffer_desc desc);
		~framebuffer();

		/**
		 * @brief Binds the framebuffer
		*/
		void bind();
		
		/**
		 * @brief Sets the framebuffer color attachment to the specified texture
		 * @param texture Texture to attach
		 * @param slot Slot for the texture
		 * @param mip Mip level of the texture (default: 0)
		*/
		void ca(hogl::relay_ptr<hogl::texture> texture, int slot, int mip = 0);
	private:
		hogl_framebuffer* m_fbo;
	};

}
