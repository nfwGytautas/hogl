#pragma once

#include <entt/entt.hpp>
#include "hogl/shared/hogl_def.hpp"
#include "hogl/core/object.hpp"
#include "hogl/entity/entity.hpp"

namespace hogl {

	class entity;

	/**
	 * @brief hogl default scene
	*/
	class HOGL_CPP_API scene {
	public:
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

		friend class entity;
	};

}
