#include "hogl_min_math.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

HOGL_NSPACE_BEGIN_L1

hogl_m44<float> hogl_look_at(const hogl_v3<float>& eye, const hogl_v3<float>& target, const hogl_v3<float>& up)
{
	// Some pointer magic to convert from hogl to glm but this should mostly be fine at performant
	// Reinterpret casts should avoid a memory copy
	return (float*)glm::value_ptr(
		glm::lookAt(
			*reinterpret_cast<const glm::vec3*>(eye.ptr_const()),
			*reinterpret_cast<const glm::vec3*>(target.ptr_const()),
			*reinterpret_cast<const glm::vec3*>(up.ptr_const())
		)
	);
}

hogl_m44<float> hogl_perspective(const float& fov, const float& aspectRatio, const float& near, const float& far)
{
	return (float*)glm::value_ptr(glm::perspective(glm::radians(fov), aspectRatio, near, far));
}

HOGL_NSPACE_END_L1
