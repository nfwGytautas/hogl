#pragma once

#include <entt/entt.hpp>
#include "hogl/shared/hogl_def.hpp"
#include "hogl/core/object.hpp"
#include "hogl/entity/scene.hpp"
#include "hogl_core/shared/hogl_log.h"

namespace hogl {

	class scene;

	/**
	 * @brief Entity object
	*/
	class HOGL_CPP_API entity {
	public:
		entity(relay_ptr<scene> scene, entt::entity id);

		/**
		 * @brief Adds a component of type T to the entity
		*/
		template<typename T, typename... Args>
		T& add_component(Args&&... args) {
#ifndef HOGL_SUPPRESS_ENTITY_CHECKS
			if (!has_component<T>()) {
				hogl_log_warn("Trying to add an already existing component!");
				return this->get_component<T>();
			}
#endif
			T& component = m_scene->m_sreg.emplace<T>(m_id, std::forward<Args>(args)...);
			return component;
		}

		/**
		 * @brief Returns the component of type T
		*/
		template<typename T>
		T& get_component() {
#ifndef HOGL_SUPPRESS_ENTITY_CHECKS
			if (!has_component<T>()) {
				hogl_log_warn("Trying to get non existing component!");
				return T();
			}
#endif
			return m_scene->m_sreg.get<T>(m_id);
		}

		/**
		 * @brief Checks if the entity has a component of type T
		*/
		template<typename T>
		bool has_component() {
			return m_scene->m_sreg.has<T>(m_id);
		}

		/**
		 * @brief Removes a component of type T from the entity
		*/
		template<typename T>
		void remove_component() {
#ifndef HOGL_SUPPRESS_ENTITY_CHECKS
			if (!has_component<T>) {
				hogl_log_warn("Trying to remove non existing component!");
				return;
			}
#endif
			m_scene->m_sreg.remove<T>(m_id);
		}

		bool operator==(const entity& other) const {
			return m_id == other.m_id && m_scene == other.m_scene;
		}

		bool operator!=(const entity& other) const {
			return !(*this == other);
		}

		/**
		 * @brief Returns the scene where the entity exists in
		*/
		relay_ptr<scene> get_scene();
	private:
		entt::entity m_id;
		relay_ptr<scene> m_scene;
	};

}
