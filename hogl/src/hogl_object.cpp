#include "hogl_object.hpp"

#include <iostream>

#include <gl/glad.h>
#include <gl/glfw3.h>

HOGL_NSPACE_BEGIN

void hogl_ubo_data(hogl_ubo* ubo, void* data, size_t offset, size_t size)
{
	// Validate size
	if (offset + size > ubo->allocated)
	{
		std::cout << "[hogl:warn] Trying to set data that is larger than the allocated size for a ubo\n";
		return;
	}

	if (size == 0)
	{
		size = ubo->allocated;
	}

	// Bind ubo
	glBindBuffer(GL_UNIFORM_BUFFER, ubo->ubo_id);

	// Map buffer, set data and unmap
	void* ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);

	// Alias the data
	char* typed_data = (char*)data;

	// Copy new data
	std::memcpy(ptr, typed_data + offset, size);

	// Unmap the buffer
	glUnmapBuffer(GL_UNIFORM_BUFFER);
}

HOGL_NSPACE_END
