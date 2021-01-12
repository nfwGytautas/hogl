/**
* @brief hogl os file is file used to abstract all the platform specific functionality that hogl uses
*/

#ifndef _HOGL_OS_
#define _HOGL_OS_

#include "hogl_core/shared/hogl_def.h"

/**
 * @brief Returns the current thread id
 * @return Current thread id
*/
unsigned long hogl_get_thread_id(void);

/**
 * @brief Sets the destination value atomically and returns the previous value
 * @param destination The value pointer
 * @param newValue New value of the variable
 * @return Previous value of the variable
*/
int hogl_atomic_set_b32(int* destination, int newValue);

/**
 * @brief Returns the variable value atomically
 * @param variable The variable you want to check
 * @return The value of the variable
*/
int hogl_atomic_get_b32(int* variable);

/**
 * @brief Adds the value to the atomic variable pointed to destination
 * @param variable Pointer to the variable
 * @param value Value to add to
 * @return The new value of destination
*/
int hogl_atomic_add_b32(int* variable, int value);

/**
 * @brief Subtracts value from the variable
 * @param variable Variable to subtract from
 * @param value Value to subtract by
 * @return New value of variable
*/
int hogl_atomic_substract_b32(int* variable, int value);

#endif