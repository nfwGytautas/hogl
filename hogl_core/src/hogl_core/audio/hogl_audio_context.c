#include "hogl_audio_context.h"

#include <AL/al.h>
#include <AL/alc.h>

#include "hogl_core/shared/hogl_log.h"
#include "hogl_core/shared/hogl_memory.h"

typedef struct _hogl_audio_manager {
	ALCdevice* device;
	ALCcontext* context;
} hogl_audio_manager;

hogl_audio_manager* audio_manager = NULL;

hogl_error hogl_audio_init(void) {
	audio_manager = hogl_malloc(sizeof(hogl_audio_manager));

	audio_manager->device = NULL;
	audio_manager->context = NULL;

	// Default device and context
	audio_manager->device = alcOpenDevice(NULL);

	if (audio_manager->device == NULL) {
		hogl_log_error("Failed to create OpenAL device");
		return HOGL_ERROR_OPENAL_DEVICE;
	}

	audio_manager->context = alcCreateContext(audio_manager->device, NULL);

	if (audio_manager->device == NULL) {
		hogl_log_error("Failed to create OpenAL device");
		return HOGL_ERROR_OPENAL_CONTEXT;
	}

	if (!alcMakeContextCurrent(audio_manager->context)) {
		hogl_log_error("Failed to make current for OpenAL context");
		return HOGL_ERROR_OPENAL_CONTEXT;
	}

	// Clear error stack
	hogl_al_check();

	return HOGL_ERROR_NONE;
}

void hogl_audio_shutdown(void) {
	alcMakeContextCurrent(NULL);
	alcDestroyContext(audio_manager->context);
	alcCloseDevice(audio_manager->device);
	hogl_free(audio_manager);
}
