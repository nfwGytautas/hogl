#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "hogl/shared/pointer.hpp"
#include "hogl/entity/mesh.hpp"
#include "hogl/entity/material.hpp"

namespace hogl {

	/**
	 * @brief Component holding information about single entity position in space
	*/
	struct HOGL_CPP_API transform_component {
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		transform_component() = default;
		transform_component(const transform_component&) = default;
	};

	/**
	 * @brief Component holding mesh object of the entity
	*/
	struct HOGL_CPP_API mesh_component {
		relay_ptr<mesh> mesh;

		mesh_component() = default;
		mesh_component(relay_ptr<hogl::mesh> amesh)
			: mesh(amesh){}
		mesh_component(const mesh_component& other) : mesh(other.mesh) {};
	};

	/**
	 * @brief Component holding entity rendering information
	*/
	struct HOGL_CPP_API renderer_component {
		relay_ptr<material> material;

		renderer_component() = default;
		renderer_component(relay_ptr<hogl::material> amaterial)
			: material(amaterial) {}
		renderer_component(const renderer_component& other) : material(other.material) {};
	};

	/**
	 * @brief Component holding light information
	*/
	struct HOGL_CPP_API light_component {
		glm::vec3 color;

		light_component() = default;
		light_component(glm::vec3 color)
			: color(color) {}
		light_component(const light_component& other) : color(other.color) {};
	};
}
