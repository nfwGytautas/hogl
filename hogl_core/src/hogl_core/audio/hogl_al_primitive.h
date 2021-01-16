/**
* @brief hogl audio primitive file similarly to gl primitive file contains OpenAL functionality abstracted for hogl
*/

#ifndef _HOGL_AUDIO_PRIMITIVE_
#define _HOGL_AUDIO_PRIMITIVE_

#include <stdbool.h>
#include "hogl_core/shared/hogl_def.h"

/**
 * @brief Defines an audio source
*/
typedef struct _hogl_asource hogl_asource;

/**
 * @brief Defines an audio data object
*/
typedef struct _hogl_abuffer hogl_abuffer;

/**
 * @brief Defines an audio data object description
*/
typedef struct _hogl_abuffer_desc {
	/**
	 * @brief Data pointer
	*/
	void* data;

	/**
	 * @brief Length of the data pointer
	*/
	size_t data_size;

	/**
	 * @brief Audio format
	*/
	hogl_audio_format format;

	/**
	 * @brief Sampling rate of the audio file
	*/
	size_t sample_rate;
} hogl_abuffer_desc;

/**
 * @brief Sets the listeners x, y, z positions, there only exist 1 single listener in the application
 * @param x X value
 * @param y Y value
 * @param z Z value
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 * 		HOGL_ERROR_OPENAL_GENERIC	if hogl_al_check failed at any point
*/
HOGL_API hogl_error hogl_listener_position(float x, float y, float z);

/**
 * @brief Sets the listeners x, y, z velocity, there only exist 1 single listener in the application
 * @param x X value
 * @param y Y value
 * @param z Z value
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 * 		HOGL_ERROR_OPENAL_GENERIC	if hogl_al_check failed at any point
*/
HOGL_API hogl_error hogl_listener_velocity(float x, float y, float z);

/**
 * @brief Sets the listeners orientation, there only exist 1 single listener in the application
 * @param value Array containing 6 floats the first 3 are the forward vector, the next 3 are the up vector
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 * 		HOGL_ERROR_OPENAL_GENERIC	if hogl_al_check failed at any point
*/
HOGL_API hogl_error hogl_listener_orientation(float* value);

/**
 * @brief Creates a new audio source
 * @param source Object to store the new source in
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 * 		HOGL_ERROR_OPENAL_GENERIC	if hogl_al_check failed at any point
*/
HOGL_API hogl_error hogl_source_new(hogl_asource** source);

/**
 * @brief Sets the source pitch to the one specified
 * @param source The source object
 * @param value New pitch value
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 * 		HOGL_ERROR_OPENAL_GENERIC	if hogl_al_check failed at any point
*/
HOGL_API hogl_error hogl_source_pitch(hogl_asource* source, float value);

/**
 * @brief Sets the source gain to the one specified
 * @param source The source object
 * @param value New gain value
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 * 		HOGL_ERROR_OPENAL_GENERIC	if hogl_al_check failed at any point
*/
HOGL_API hogl_error hogl_source_gain(hogl_asource* source, float value);

/**
 * @brief Sets the source position to the one specified
 * @param source The source object
 * @param x X component
 * @param y Y component
 * @param z Z component
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 * 		HOGL_ERROR_OPENAL_GENERIC	if hogl_al_check failed at any point
*/
HOGL_API hogl_error hogl_source_position(hogl_asource* source, float x, float y, float z);

/**
 * @brief Sets the source velocity to the one specified
 * @param source The source object
 * @param x X component
 * @param y Y component
 * @param z Z component
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 * 		HOGL_ERROR_OPENAL_GENERIC	if hogl_al_check failed at any point
*/
HOGL_API hogl_error hogl_source_velocity(hogl_asource* source, float x, float y, float z);

/**
 * @brief Sets the source loop value
 * @param source The source object
 * @param value True to loop, false to not loop
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 * 		HOGL_ERROR_OPENAL_GENERIC	if hogl_al_check failed at any point
*/
HOGL_API hogl_error hogl_source_loop(hogl_asource* source, bool value);

/**
 * @brief Starts playing the source
 * @param source The source object
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 * 		HOGL_ERROR_OPENAL_GENERIC	if hogl_al_check failed at any point
*/
HOGL_API hogl_error hogl_source_play(hogl_asource* source);

/**
 * @brief Free the source object
 * @param source Object to free
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 * 		HOGL_ERROR_OPENAL_GENERIC	if hogl_al_check failed at any point
*/
HOGL_API hogl_error hogl_source_free(hogl_asource* source);

/**
 * @brief Creates a new audio buffer from the specified description
 * @param buffer Pointer where to store the new buffer
 * @param desc Description of the buffer
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 * 		HOGL_ERROR_OPENAL_GENERIC	if hogl_al_check failed at any point
*/
HOGL_API hogl_error hogl_abuffer_new(hogl_abuffer** buffer, hogl_abuffer_desc desc);

/**
 * @brief Frees the specified audio buffer
 * @param buffer Buffer to free
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 * 		HOGL_ERROR_OPENAL_GENERIC	if hogl_al_check failed at any point
*/
HOGL_API hogl_error hogl_abuffer_free(hogl_abuffer* buffer);

/**
 * @brief Sets the audio source data to be equal to the specified buffer
 * @param source Source object to link the buffer to
 * @param buffer Buffer object to link to the source
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 * 		HOGL_ERROR_OPENAL_GENERIC	if hogl_al_check failed at any point
*/
HOGL_API hogl_error hogl_source_buffer(hogl_asource* source, hogl_abuffer* buffer);

#endif