#pragma once

#include <array>
#include "hogl/shared/hogl_def.hpp"
#include "hogl/shared/pointer.hpp"
#include "hogl/core/object.hpp"
#include "hogl/core/texture.hpp"
#include "hogl/core/shader.hpp"

namespace hogl {

	/**
	 * @brief A generic material class that can be used to send textures to associated shader
	 * for single textures a texture atlas or raw hogl C API will need to be used
	*/
	class HOGL_CPP_API material : public object {
		HOGL_OBJECT(2);
	public:
		/**
		 * @brief Sets the material texture in the specified slot
		 * @param texture Texture to set
		 * @param slot Which slot to set to
		*/
		void set_texture(const ref<texture>& texture, size_t slot);

		/**
		 * @brief Sets the material shader
		 * @param shader Shader to set
		*/
		void set_shader(const ref<shader>& shader);

		/**
		 * @brief Binds the material
		*/
		void bind();
	private:
		std::array<ref<texture>, MAX_TEXTURE_UNITS> m_textures;
		ref<shader> m_shader;
	};

}
