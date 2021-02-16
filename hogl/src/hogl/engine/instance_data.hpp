#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

#include "hogl/shared/hogl_def.hpp"
#include "hogl/shared/pointer.hpp"
#include "hogl/core/object.hpp"
#include "hogl/core/gpu_buffer.hpp"

namespace hogl {

	/**
	 * @brief Class used to pass instance rendering data to shaders
	*/
	class HOGL_CPP_API instance_data {
	public:
		instance_data();
		~instance_data();

		/**
		 * @brief Push a vector of transformation into the instance rendering data
		 * @param transformations Transformations to set
		*/
		void push_transformations(std::vector<glm::mat4>& transformations);

		/**
		 * @brief Binds the data to current VAO
		*/
		void bind();
	private:
		gpu_buffer* m_model;
	};

}
