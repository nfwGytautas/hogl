#include "hogl_render.h"

#include <gl/glad.h>
#include <gl/glfw3.h>

#include "hogl_core/hogl_log.h"
#include "hogl_core/hogl_memory.h"

unsigned int __parse_mode(hogl_render_mode mode) {
	switch (mode) {
	case HOGL_RM_TRIANGLES:
		return GL_TRIANGLES;
	case HOGL_RM_TRIANGLE_STRIP:
		return GL_TRIANGLE_STRIP;
	}

	return 0;
}

void hogl_render_clear(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

hogl_rstate hogl_default_rstate(void)
{
	hogl_rstate rstate = {0};

	rstate.depth = HOGL_RD_LEQUAL;

	return rstate;
}

void hogl_set_rstate(hogl_rstate rstate)
{
	return HOGL_ERROR_NONE;
}

void hogl_set_depth_test(hogl_render_depth depth)
{
	if (depth == HOGL_RD_DISABLED)
	{
		glDisable(GL_DEPTH_TEST);
		return;
	}

	// Set depth function
	glEnable(GL_DEPTH_TEST);
	switch (depth)
	{
	case HOGL_RD_ALWAYS:
		glDepthFunc(GL_ALWAYS);
		break;
	case HOGL_RD_NEVER:
		glDepthFunc(GL_NEVER);
		break;
	case HOGL_RD_LESS:
		glDepthFunc(GL_LESS);
		break;
	case HOGL_RD_EQUAL:
		glDepthFunc(GL_EQUAL);
		break;
	case HOGL_RD_LEQUAL:
		glDepthFunc(GL_LEQUAL);
		break;
	case HOGL_RD_GREATER:
		glDepthFunc(GL_GREATER);
		break;
	case HOGL_RD_NOTEQUAL:
		glDepthFunc(GL_NOTEQUAL);
		break;
	case HOGL_RD_GEQUAL:
		glDepthFunc(GL_GEQUAL);
		break;
	}
}

hogl_error hogl_render_a(hogl_render_mode mode, unsigned int vertices) {
#ifndef HOGL_DISABLE_GL_WARNING
	if (__parse_mode(mode) == 0) {
		hogl_log_error("Failed to parse a render mode: %ld", mode);
		return HOGL_ERROR_UNKNOWN_MODE;
	}
#endif

	glDrawArrays(__parse_mode(mode), 0, vertices);
	return HOGL_ERROR_NONE;
}

hogl_error hogl_render_e(hogl_render_mode mode, unsigned int vertices) {
#ifndef HOGL_DISABLE_GL_WARNING
	if (__parse_mode(mode) == 0) {
		hogl_log_error("Failed to parse a render mode: %ld", mode);
		return HOGL_ERROR_UNKNOWN_MODE;
	}
#endif

	glDrawElements(__parse_mode(mode), vertices, GL_UNSIGNED_INT, 0);
	return HOGL_ERROR_NONE;
}

hogl_error hogl_render_ai(hogl_render_mode mode, unsigned int vertices, unsigned int instances) {
#ifndef HOGL_DISABLE_GL_WARNING
	if (__parse_mode(mode) == 0) {
		hogl_log_error("Failed to parse a render mode: %ld", mode);
		return HOGL_ERROR_UNKNOWN_MODE;
	}
#endif
	
	glDrawArraysInstanced(__parse_mode(mode), 0, vertices, instances);
	return HOGL_ERROR_NONE;
}

hogl_error hogl_render_ei(hogl_render_mode mode, unsigned int vertices, unsigned int instances) {
#ifndef HOGL_DISABLE_GL_WARNING
	if (__parse_mode(mode) == 0) {
		hogl_log_error("Failed to parse a render mode: %ld", mode);
		return HOGL_ERROR_UNKNOWN_MODE;
	}
#endif

	glDrawElementsInstanced(__parse_mode(mode), vertices, GL_UNSIGNED_INT, 0, instances);
	return HOGL_ERROR_NONE;
}
