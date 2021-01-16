/**
* @brief hogl core file used to import the library
*/

#ifndef _HOGL_CORE_
#define _HOGL_CORE_

/**
* Some hogl functionality can be disabled by passing preprocessor flags, by default all of functionality is enabled
* these are all the flags that can be passed:
*
* HOGL_SUPPRESS_GL_LOG				Suppresses all OpenGL error logging
* HOGL_DISABLE_MEM_TRACK			Disables memory allocation tracking
* HOGL_DISABLE_GL_BOUND_CHECKING	Disables bound checking for vertex buffer objects
* HOGL_DISABLE_GL_WARNING			Disables warning that occur when hogl detects an anomaly for example when the user tries to bind a vao before setting all data for vbos
* HOGL_ENALBE_ALL_GL_LOGS			Enables all by default ignored error messages
* HOGL_DISABLE_AL_WARNING			Disables warning that occur from OpenAL
*/

/**
* Files
*/
#include <stdbool.h>
#include <stddef.h>

#include "hogl_core/shared/hogl_def.h"
#include "hogl_core/shared/hogl_memory.h"
#include "hogl_core/shared/hogl_log.h"

#ifdef HOGL_SUITE_GRAPHICS
#include "hogl_core/graphics/hogl_wnd.h"
#include "hogl_core/graphics/hogl_gl_primitive.h"
#include "hogl_core/graphics/hogl_render.h"
#endif

#ifdef HOGL_SUITE_AUDIO
#include "hogl_core/audio/hogl_audio_context.h"
#include "hogl_core/audio/hogl_al_primitive.h"
#endif

#ifdef HOGL_SUITE_VF
#include "hogl_core/io/hogl_vf.h"
#endif

/**
* Functionality
*/

/**
 * @brief Returns error message description, if the error is HOGL_ERROR_NONE than the return is NULL
 * @param err Error whose description is needed
 * @return Description string
*/
HOGL_API const char* hogl_error_desc(hogl_error err);

/**
 * @brief Initializes hogl and all its subsystems
 * @return Returns error codes:
 *		HOGL_ERROR_NONE			if initialization was successful
 *		HOGL_ERROR_GLFW_INIT	if glfw could not be initialized
*/
HOGL_API hogl_error hogl_init(void);

/**
 * @brief Shutdowns hogl library
*/
HOGL_API void hogl_shutdown(void);

#endif