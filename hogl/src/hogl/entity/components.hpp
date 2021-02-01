#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace hogl {

	/**
	 * @brief Component holding information about single entity position in space
	*/
	struct transform_component {
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		transform_component() = default;
		transform_component(const transform_component&) = default;
	};

}
