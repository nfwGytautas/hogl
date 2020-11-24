
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
	HOGL_ERROR_NONE
} hogl_error;


/**
* Type forward declarations
*/


/**
* General types
*/


/**
* Files
*/
#include <stdbool.h>
#include "hogl_core/hogl_log.h"

/**
* Functionality
*/

/**
 * @brief Returns error message description, if the error is HOGL_ERROR_NONE than the return is NULL
 * @param err Error whose description is needed
 * @return Description string
*/
HOGL_API const char* hogl_error_desc(hogl_error err);

HOGL_API hogl_error hogl_init();

#endif