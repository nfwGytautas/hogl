/**
* @brief hogl audio context file contains functions used to initialize OpenAL context and devices
*/

#ifndef _HOGL_AUDIO_CONTEXT_
#define _HOGL_AUDIO_CONTEXT_

#include "hogl_core/shared/hogl_def.h"

/**
 * @brief Initializes the OpenAL with default device and context
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 *		HOGL_ERROR_OPENAL_DEVICE	if a OpenAL device could not have been created
 *		HOGL_ERROR_OPENAL_CONTEXT	if OpenAL context could not be created for the device or it failed to be made current
 *		HOGL_ERROR_OPENAL_GENERIC	if hogl_al_check failed at any point
*/
HOGL_API hogl_error hogl_audio_init(void);

/**
 * @brief Shuts down the audio manager and frees memory
*/
HOGL_API void hogl_audio_shutdown(void);

#endif