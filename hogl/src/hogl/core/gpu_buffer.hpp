#pragma once

#include <vector>
#include "hogl/shared/hogl_def.hpp"
#include "hogl/core/object.hpp"
#include "hogl_core/graphics/hogl_gl_primitive.h"

namespace hogl {

	/**
	 * @brief Wrapper around hogl_vbo, so that it could be used with a ref pointer
	*/
	class HOGL_CPP_API gpu_buffer : public object {
		HOGL_OBJECT(4);
	public:
		gpu_buffer(const hogl_vbo_desc& desc);
		~gpu_buffer();

		/**
		 * @brief Sets the buffer internal data
		 * @tparam T Type of data
		 * @param data New data, will be resized if no space left
		 * @param offset Offset of the new buffer
		*/
		template<typename T>
		void set_data(std::vector<T>& data, size_t offset = 0) {
			set_data_generic(static_cast<void*>(data.data()), data.size() * sizeof(T), offset);
		}

		/**
		 * @brief Returns the amount of elements inside the buffer
		*/
		size_t count();

		/**
		 * @brief Gets the type of the buffer
		*/
		hogl_vbo_type type();

		/**
		 * @brief Binds the buffer
		*/
		void bind();
	private:
		void set_data_generic(void* data, size_t len, size_t offset);
	private:
		hogl_vbo* m_vbo = nullptr;
		size_t m_stride = 0;
		size_t m_len = 0;
		hogl_vbo_type m_type;
	};
}
