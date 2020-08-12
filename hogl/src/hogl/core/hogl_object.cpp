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

void hogl_adjust_rbo(hogl_framebuffer* fbo, hogl_rbuffer_format component, unsigned int width, unsigned int height)
{
	// Check if there is a valid rbo
	if (fbo->rbo_id == 0)
	{
		HOGL_LOG_ERROR("No valid renderbuffer for the specified fbo");
		return;
	}

	// Bind fbo and rbo
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo_id);
	glBindRenderbuffer(GL_RENDERBUFFER, fbo->rbo_id);

	// Make into an actual function
	GLenum format;
	switch (component)
	{
	case hogl_rbuffer_format::d16:
		format = GL_DEPTH_COMPONENT16;
		break;
	case hogl_rbuffer_format::d24:
		format = GL_DEPTH_COMPONENT24;
		break;
	case hogl_rbuffer_format::d32F:
		format = GL_DEPTH_COMPONENT32F;
		break;
	case hogl_rbuffer_format::d24_s8:
		format = GL_DEPTH24_STENCIL8;
		break;
	case hogl_rbuffer_format::d32F_s8:
		format = GL_DEPTH32F_STENCIL8;
		break;
	case hogl_rbuffer_format::si8:
		format = GL_STENCIL_INDEX8;
		break;
	}

	glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
}

HOGL_NSPACE_END
