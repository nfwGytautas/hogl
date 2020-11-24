/**
* @brief hogl error file, this is used to setup the error struct and some error specific functionality
*/

#ifndef _HOGL_LOG_
#define _HOGL_LOG_

#include "hogl_core.h"

/**
* Actual use functions
*/

#define hogl_log_error(msg) _hogl_log_error(__LINE__, __func__, msg);
#define hogl_log_warn(msg) _hogl_log_warn(__LINE__, __func__, msg);
#define hogl_log_info(msg) _hogl_log_info(__LINE__, __func__, msg);
#define hogl_log_trace(msg) _hogl_log_trace(__LINE__, __func__, msg);

/**
* Implementation functions
*/

HOGL_API void _hogl_log_error(int line, const char* function, const char* msg);
HOGL_API void _hogl_log_warn(int line, const char* function, const char* msg);
HOGL_API void _hogl_log_info(int line, const char* function, const char* msg);
HOGL_API void _hogl_log_trace(int line, const char* function, const char* msg);

#endif