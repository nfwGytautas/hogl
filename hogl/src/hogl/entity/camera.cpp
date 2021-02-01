#include "camera.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace hogl {
	camera::camera(float fov, float nearclip, float farclip, unsigned int vwidth, unsigned int vheight)
		: m_fov(fov), m_nearclip(nearclip), m_farclip(farclip), m_aspect((float)vwidth / (float)vheight)
	{
	}

	void camera::reposition(glm::vec3 position) {
		m_position = position;
	}

	void camera::look_at(glm::vec3 position) {
		//glm::vec3 rot = glm::normalize(m_position - position) * 360.0f;
		glm::vec3 ndir = glm::normalize(position - m_position);

		m_pitch = glm::degrees(glm::asin(ndir.y));
		m_yaw = glm::degrees(glm::asin(ndir.z));
	}

	void camera::viewport(unsigned int width, unsigned height) {
		m_aspect = width / height;
	}

	glm::mat4 camera::compute_view() const {
		// Calculate the direction of the camera from the rotation
		glm::vec3 direction;
		direction.x = glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
		direction.y = glm::sin(glm::radians(m_pitch));
		direction.z = glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));

		// Normalize direction to get camera front vector
		glm::vec3 front = glm::normalize(direction);

		return glm::lookAt(m_position, m_position + front, m_up);
	}

	glm::mat4 camera::compute_projection() const {
		return glm::perspective(glm::radians(m_fov), m_aspect, m_nearclip, m_farclip);
	}

	glm::vec3 camera::up() const {
		return m_up;
	}

	void camera::up(glm::vec3 value) {
		m_up = value;
	}

}
