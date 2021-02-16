#pragma once

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include "hogl/shared/hogl_def.hpp"
#include "hogl/shared/pointer.hpp"
#include "hogl/core/ubo.hpp"
#include "hogl/core/shader.hpp"

namespace hogl {

	class per_pass_ubo;
	class camera_ubo;
	class light_ubo;

	/**
	 * @brief Struct containing all standard hogl UBOs
	*/
	class HOGL_CPP_API std_ubo_pkg {
	public:
		relay_ptr<per_pass_ubo> per_pass;
		relay_ptr<camera_ubo> camera;
		relay_ptr<light_ubo> light;

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
		struct _data {
			glm::mat4 projection;
			glm::mat4 view;
		} data = {};

		per_pass_ubo() 
			: ubo("per_pass", 0, sizeof(_data))
		{
			p_iBuff = &data;
		}

		/**
		 * Member access operator
		 */
		_data* operator->() {
			return &data;
		}
	};

	/**
	 * @brief Camera UBO contains data that is used to provide camera position and other related data to shaders
	*/
	class HOGL_CPP_API camera_ubo : public ubo {
	public:
		struct _data {
			glm::vec3 position;
			float padding;
		} data = {};

		camera_ubo()
			: ubo("camera_ubo", 1, sizeof(_data))
		{
			p_iBuff = &data;
		}

		/**
		 * Member access operator
		 */
		_data* operator->() {
			return &data;
		}
	};

	/**
	 * @brief Light UBO contains data that is used to provide light information to shaders
	*/
	class HOGL_CPP_API light_ubo : public ubo {
	public:
		// Must be dividable by 2 to conform to 4 byte alignment
		const static unsigned int MAX_LIGHTS = 16;

		struct _data {
			unsigned int count;
			glm::vec3 _padding;

			glm::vec4 position[MAX_LIGHTS];
			glm::vec4 colors[MAX_LIGHTS];
		} data = {};

		light_ubo()
			: ubo("light_ubo", 2, sizeof(_data))
		{
			p_iBuff = &data;
		}

		/**
		 * Member access operator
		 */
		_data* operator->() {
			return &data;
		}
	};
}
