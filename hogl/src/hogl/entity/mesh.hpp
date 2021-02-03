#pragma once

#include <vector>
#include "hogl/shared/hogl_def.hpp"
#include "hogl/shared/pointer.hpp"
#include "hogl/core/object.hpp"
#include "hogl/core/gpu_buffer.hpp"

#include "hogl_core/graphics/hogl_gl_primitive.h"

namespace hogl {

	/**
	 * @brief A mesh class provided out of the box by hogl used to wrap some vao functionality
	*/
	class HOGL_CPP_API mesh : public object {
		HOGL_OBJECT(1);
	public:
		mesh();
		~mesh();

		/**
		 * @brief Sets the way the mesh is rendered
		 * @param rmode Rendering mode
		*/
		void set_render_mode(hogl_render_mode rmode);

		/**
		 * @brief Adds the specified buffer to the mesh
		 * @param buffer Buffer to add
		*/
		void add_buffer(const ref<gpu_buffer>& buffer);

		/**
		 * @brief Binds the mesh
		*/
		void bind();

		/**
		 * @brief Renders the mesh
		 * TODO: Maybe move to dedicated renderer
		*/
		void render();
	private:
		hogl_render_mode m_rmode = HOGL_RM_TRIANGLES;
		std::vector<ref<gpu_buffer>> m_buffers = {};
		hogl_vao* m_vao = nullptr;
		size_t m_vcount = 0;
		size_t m_ecount = 0;
	};

}
