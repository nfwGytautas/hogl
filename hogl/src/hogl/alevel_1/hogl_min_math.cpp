#include "hogl_min_math.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

HOGL_NSPACE_BEGIN_L1

hogl_m44<float> hogl_m44_empty()
{
	return (float*)glm::value_ptr(
		glm::mat4(1.0f)
	);
}

hogl_m44<float> hogl_look_at(const hogl_v3<float>& eye, const hogl_v3<float>& target, const hogl_v3<float>& up)
{
	// Some pointer magic to convert from hogl to glm but this should mostly be fine at performant
	// Reinterpret casts should avoid a memory copy

	glm::vec3 position = glm::make_vec3(eye.ptr_const());
	glm::vec3 front = glm::make_vec3(target.ptr_const());
	glm::vec3 glm_up = glm::make_vec3(up.ptr_const());

	return (float*)glm::value_ptr(
		glm::lookAt(
			position,
			position + front,
			glm_up
		)
	);
}

hogl_m44<float> hogl_perspective(const float& fov, const float& aspectRatio, const float& near, const float& far)
{
	return (float*)glm::value_ptr(glm::perspective(glm::radians(fov), aspectRatio, near, far));
}

hogl_m44<float> hogl_translate(const hogl_m44<float>& matrix, const hogl_v3<float> value)
{
	return (float*)glm::value_ptr(glm::translate(glm::make_mat4(matrix.ptr_const()), glm::make_vec3(value.ptr_const())));
}

hogl_m44<float> hogl_scale(const hogl_m44<float>& matrix, const hogl_v3<float> value)
{
	return (float*)glm::value_ptr(glm::scale(glm::make_mat4(matrix.ptr_const()), glm::make_vec3(value.ptr_const())));
}

HOGL_NSPACE_END_L1
