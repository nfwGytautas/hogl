
/**
* @brief hogl core file, this is used to setup defines, types, and general functionality
*/

#ifndef _HOGL_CORE_
#define _HOGL_CORE_

// HOGL_API is used to allow C++ compilers to use this library without extra 
#ifndef HOGL_DEVELOPMENT
	// If C++ compiler setup C linkage
	#ifdef __cplusplus
		#ifdef HOGL_EXPORT
			#define HOGL_API extern "C" __declspec(dllexport)
		#else
			#define HOGL_API extern "C" __declspec(dllimport)
		#endif
	#else
		#define HOGL_API
	#endif
#else
	#define HOGL_API
#endif

#ifndef NULL
#define NULL 0
#endif

/**
* Enums
*/

/**
 * @brief hogl_error enum is used to see if there was an error while performing a function.
 * If the value is not equal to HOGL_ERROR_NONE, that means there was an error in the last call.
 * Additional information will be outputted into the screen automatically.
*/
typedef enum {
	HOGL_ERROR_NONE,
	HOGL_ERROR_GLFW_INIT,
	HOGL_ERROR_WND_CREATE,
	HOGL_ERROR_GLAD_INIT,
} hogl_error;

/**
* Files
*/
#include <stdbool.h>
#include "hogl_core/hogl_memory.h"
#include "hogl_core/hogl_log.h"
#include "hogl_core/hogl_wnd.h"

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