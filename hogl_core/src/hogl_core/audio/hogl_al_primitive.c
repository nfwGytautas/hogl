#include "hogl_al_primitive.h"

#include <AL/al.h>
#include <AL/alc.h>

#include "hogl_core/shared/hogl_log.h"
#include "hogl_core/shared/hogl_memory.h"

typedef struct _hogl_asource {
	unsigned int id;
} hogl_asource;

typedef struct _hogl_abuffer {
	unsigned int id;
} hogl_abuffer;

hogl_error hogl_listener_position(float x, float y, float z) {
	alListener3f(AL_POSITION, x, y, z);
	hogl_al_check();
	return HOGL_ERROR_NONE;
}

hogl_error hogl_listener_velocity(float x, float y, float z) {
	alListener3f(AL_VELOCITY, x, y, z);
	hogl_al_check();
	return HOGL_ERROR_NONE;
}

hogl_error hogl_listener_orientation(float* value) {
	alListenerfv(AL_ORIENTATION, value);
	hogl_al_check();
	return HOGL_ERROR_NONE;
}

hogl_error hogl_source_new(hogl_asource** source) {
	(*source) = hogl_malloc(sizeof(hogl_asource));

	alGenSources(1, &(*source)->id);
	hogl_al_check();

	return HOGL_ERROR_NONE;
}

hogl_error hogl_source_pitch(hogl_asource* source, float value) {
	alSourcef(source->id, AL_PITCH, value);
	hogl_al_check();
	return HOGL_ERROR_NONE;
}

hogl_error hogl_source_gain(hogl_asource* source, float value) {
	alSourcef(source->id, AL_GAIN, value);
	hogl_al_check();
	return HOGL_ERROR_NONE;
}

hogl_error hogl_source_position(hogl_asource* source, float x, float y, float z) {
	alSource3f(source->id, AL_POSITION, x, y, z);
	hogl_al_check();
	return HOGL_ERROR_NONE;
}

hogl_error hogl_source_velocity(hogl_asource* source, float x, float y, float z) {
	alSource3f(source->id, AL_VELOCITY, x, y, z);
	hogl_al_check();
	return HOGL_ERROR_NONE;
}

hogl_error hogl_source_loop(hogl_asource* source, bool value) {
	alSourcei(source->id, AL_LOOPING, value ? AL_TRUE : AL_FALSE);
	hogl_al_check();
	return HOGL_ERROR_NONE;
}

hogl_error hogl_source_play(hogl_asource* source) {
	alSourcePlay(source->id);
	hogl_al_check();
	return HOGL_ERROR_NONE;
}

hogl_error hogl_source_free(hogl_asource* source) {
	alDeleteSources(1, &source->id);
	hogl_al_check();
	hogl_free(source);
	return HOGL_ERROR_NONE;
}

hogl_error hogl_abuffer_new(hogl_abuffer** buffer, hogl_abuffer_desc desc) {
	ALenum format = 0;

	(*buffer) = hogl_malloc(sizeof(hogl_abuffer));
	alGenBuffers(1, &(*buffer)->id);
	hogl_al_check();

	switch (desc.format) {
	case HOGL_AF_MONO8:
		format = AL_FORMAT_MONO8;
		break;
	case HOGL_AF_MONO16:
		format = AL_FORMAT_MONO16;
		break;
	case HOGL_AF_STEREO8:
		format = AL_FORMAT_STEREO8;
		break;
	case HOGL_AF_STEREO16:
		format = AL_FORMAT_STEREO16;
		break;
	default:
		hogl_log_error("Unknown audio format")
		return HOGL_ERROR_BAD_ARGUMENT;
	}

	alBufferData((*buffer)->id, format, desc.data, desc.data_size, desc.sample_rate);
	hogl_al_check();

	return HOGL_ERROR_NONE;
}

hogl_error hogl_abuffer_free(hogl_abuffer* buffer) {
	alDeleteBuffers(1, &buffer->id);
	hogl_al_check();
	hogl_free(buffer);
	return HOGL_ERROR_NONE;
}

hogl_error hogl_source_buffer(hogl_asource* source, hogl_abuffer* buffer) {
	alSourcei(source->id, AL_BUFFER, buffer->id);
	hogl_al_check();
	return HOGL_ERROR_NONE;
}
