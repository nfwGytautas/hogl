#include "hogl_def.h"

#include <gl/glad.h>
#include <gl/glfw3.h>

#include "hogl_core/shared/hogl_log.h"

void hogl_gl_check(void) {
#ifndef HOGL_DISABLE_GL_WARNING
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		hogl_log_error("OpenGL error detected %ld", err);
	}
#endif
}
