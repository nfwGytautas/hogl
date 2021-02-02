#pragma once

#include <entt/entt.hpp>
#include "hogl/shared/hogl_def.hpp"
#include "hogl/core/object.hpp"
#include "hogl/entity/entity.hpp"
#include "hogl/entity/camera.hpp"

namespace hogl {

	class entity;
	class camera;

	/**
	 * @brief hogl default scene
	*/
	class HOGL_CPP_API scene {
	public:
		scene();

		/**
		 * @brief Sets the scene active camera to the specified (copy is made)
		 * @param cam Camera to set to
		*/
		void set_camera(const camera& cam);

		/**
		 * @brief Renders the scene
		*/
		void render();

		/**
		 * @brief Spawns a new entity for the scene
		 * @return New entity instance
		*/
		entity spawn_entity();
	private:
		entt::registry m_sreg;
		camera m_camera;

		friend class entity;
	};

}
