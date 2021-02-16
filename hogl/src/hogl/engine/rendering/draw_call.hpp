#pragma once

#include <glm/glm.hpp>

#include "hogl/shared/hogl_def.hpp"
#include "hogl/shared/pointer.hpp"
#include "hogl/entity/mesh.hpp"
#include "hogl/entity/material.hpp"

namespace hogl {

	/**
	 * @brief Struct containing draw call information
	*/
	struct HOGL_CPP_API draw_call {
		relay_ptr<mesh> mesh;
		relay_ptr<material> material;
		std::vector<glm::mat4> transformations;

		draw_call(relay_ptr<hogl::mesh> mesh, relay_ptr<hogl::material> material) 
			: mesh(mesh), material(material) {}
	};

}
