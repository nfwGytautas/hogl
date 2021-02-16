#pragma once

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "hogl/shared/hogl_def.hpp"

namespace hogl {

	/**
	 * @brief Default camera class
	*/
	class HOGL_CPP_API camera {
	public:
		camera();

		/**
		 * @brief Initializes a camera with specified initial values
		 * @param fov FoV of the camera
		 * @param nearclip Near clipping plane
		 * @param farclip Far clipping plane
		 * @param vwidth Viewport width
		 * @param vheight Viewport height
		*/
		camera(float fov, float nearclip, float farclip, unsigned int vwidth, unsigned int vheight);

		/**
		 * @brief Repositions the camera to the specified position
		 * @param position New position of the camera
		*/
		void reposition(glm::vec3 position);

		/**
		 * @brief Makes the camera look at the specified position
		 * @param position Position of the target
		*/
		void look_at(glm::vec3 position);

		/**
		 * @brief Set the viewport for the camera
		 * @param width Width of the viewport
		 * @param height Height of the viewport
		*/
		void viewport(unsigned int width, unsigned height);

		/**
		 * @brief Computes a view matrix from the camera
		 * @return glm::mat4 object computed from the camera
		*/
		glm::mat4 compute_view() const;

		/**
		 * @brief Computes a projection matrix from the camera
		 * @return glm::mat4 object computed from the camera
		*/
		glm::mat4 compute_projection() const;

		/**
		 * @brief Returns the current camera up vector
		*/
		glm::vec3 up() const;

		/**
		 * @brief Sets the camera's up direction
		*/
		void up(glm::vec3 value);

		/**
		 * @brief Returns the camera position
		*/
		glm::vec3 get_position() const;
	private:
		float m_fov = 45.0f;
		float m_aspect = 1.778f;
		float m_nearclip = 0.1f;
		float m_farclip = 1000.0f;

		float m_pitch = 0.0f;
		float m_yaw = 0.0f;
		float m_roll = 0.0f;

		glm::vec3 m_position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_up = { 0.0f, 1.0f, 0.0f };
	};

}
