#pragma once

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include "hogl/shared/hogl_def.hpp"
#include "hogl/shared/pointer.hpp"
#include "hogl/core/ubo.hpp"
#include "hogl/core/shader.hpp"

namespace hogl {

	class per_pass_ubo;

	/**
	 * @brief Struct containing all standard hogl UBOs
	*/
	class HOGL_CPP_API std_ubo_pkg {
	public:
		relay_ptr<per_pass_ubo> per_pass;

		/**
		 * @brief Attaches all standard ubos to the specified shader
		 * @param shader Sahder to attach to
		*/
		void attach_to(relay_ptr<shader> shader);
	};

	/**
	 * @brief Per pass UBO contains data that is set and updated once every frame
	*/
	class HOGL_CPP_API per_pass_ubo : public ubo {
	public:
		struct data {
			glm::mat4 projection;
			glm::mat4 view;
		} data = {};

		per_pass_ubo() 
			: ubo("per_pass", 0, sizeof(data))
		{
			p_iBuff = &data;
		}
	};

}
