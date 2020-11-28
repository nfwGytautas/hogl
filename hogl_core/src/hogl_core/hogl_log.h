/**
* @brief hogl error file, this is used to setup the error struct and some error specific functionality
*/

#ifndef _HOGL_LOG_
#define _HOGL_LOG_

#include "hogl_core.h"

/**
* Type forward declarations
*/
typedef void (*hogl_log_message_cb)(char* message, unsigned int size);

/**
* Actual use functions
*/

#define hogl_log_error(fmt, ...) hogl_log_impl("\x1b[31m", "ERROR", __LINE__, __func__, fmt, __VA_ARGS__);
#define hogl_log_warn(fmt, ...) hogl_log_impl("\x1b[33m", "WARNING", __LINE__, __func__, fmt, __VA_ARGS__);
#define hogl_log_info(fmt, ...) hogl_log_impl("\x1b[36m", "INFO", __LINE__, __func__, fmt, __VA_ARGS__);
#define hogl_log_trace(fmt, ...) hogl_log_impl("\x1b[90m", "TRACE", __LINE__, __func__, fmt, __VA_ARGS__);

/**
* Implementation functions
*/

/**
 * @brief Implementation of hogl logging function
 * @param color ASCII color code
 * @param type Text that is displayed before the message
 * @param line Line number
 * @param function Function name
 * @param format Format of the message
 * @param ... Format parameters
 * @return 
*/
HOGL_API void hogl_log_impl(const char* color, const char* type, int line, const char* function, const char* format, ...);

/**
 * @brief Sets the callback for logging if no callback is set then all output is directed to the default output, use full when you want
 * to output into a file or have a special display method. By default output will be outputted into the console window using printf.
 * This function might be called by multiple threads so make sure to make handling asynchronous also if logging to file open it first and keep
 * it opened so as to not slow down the entire logging process.
 * @param callback Function that will be called with the message and size. The message doesn't have new line character at the end.
*/
HOGL_API void hogl_set_log_cb(hogl_log_message_cb callback);

#endif