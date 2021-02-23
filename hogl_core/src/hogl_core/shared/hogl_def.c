#include "hogl_def.h"

#include <gl/glad.h>
#include <gl/glfw3.h>

#include <AL/al.h>
#include <AL/alc.h>

#include "hogl_core/shared/hogl_log.h"

int __hogl_gl_check(void) {
#ifndef HOGL_DISABLE_GL_WARNING
	GLenum err = glGetError();
	int err_count = 0;
	while (err != GL_NO_ERROR) {
		hogl_log_error("OpenGL error detected %ld", err);
		err_count++;
		err = glGetError();
	}

	return err_count;
#else
	return 0;
#endif
}

int __hogl_al_check(void) {
#ifndef HOGL_DISABLE_AL_WARNING
	ALCenum err;
	int err_count = 0;
	if ((err = alGetError()) != GL_NO_ERROR) {
		switch (err) {
		case AL_INVALID_NAME:
			hogl_log_error("OpenAL error INVALID_NAME error detected");
		case AL_INVALID_OPERATION:
			hogl_log_error("OpenAL error INVALID_OPERATION error detected");
		default:
			hogl_log_error("OpenAL error detected %ld", err);
		}
		err_count++;
	}
	return err_count;
#else
	return 0;
#endif
}
