#pragma once

#include "hogl/shared/hogl_def.hpp"
#include "hogl/core/object.hpp"

#include "hogl_core/graphics/hogl_gl_primitive.h"

namespace hogl {

	/**
	 * @brief Thin wrapper around hogl C API texture
	*/
	class HOGL_CPP_API texture : public object {
		HOGL_OBJECT(3);
	protected:
		texture();
	public:
		texture(hogl_texture_desc desc);
		~texture();

		/**
		 * @brief Sets the texture data of the object
		 * @param data New texture data
		*/
		void set_data(hogl_texture_data& data);

		/**
		 * @brief Generate a mipmap for the texture
		*/
		void gen_mipmap();

		/**
		 * @brief Binds the texture
		*/
		void bind(unsigned int slot);
	protected:
		hogl_texture* m_texture = nullptr;
		friend class framebuffer;
	};

	/**
	 * @brief Very similar to the texture object except for some additional functions to change 
	 * cubemap sides
	*/
	class HOGL_CPP_API cubemap : public texture {
	public:
		cubemap(hogl_texture_desc desc);
		~cubemap();

		/**
		 * @brief Sets the active side of the cubemap
		 * @param side Side of the cubemap
		*/
		void set_side(hogl_cm_side side);
	};

}
