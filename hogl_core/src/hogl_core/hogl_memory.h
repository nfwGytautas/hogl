/**
* @brief hogl memory file contains functionality and types used for working with memory, 
* all function will map to their standard operating system function if memory tracking is disabled
* which it should be if building for the final release, memory tracking is a debugging tool. For 
* disabling of memory tracking look at hogl define flags. Some operating system don't have memory
* tracking functionality.
*/

#ifndef _HOGL_MEMORY_
#define _HOGL_MEMORY_

#include "hogl_core.h"

/**
 * @brief Allocates size amount of bytes
 * @param size Number of bytes to allocate
 * @return Pointer to the start of the allocated range
*/
HOGL_API void* hogl_malloc(unsigned int size);

/**
 * @brief Copies data from src to dst
 * @param dst Destination pointer
 * @param src Source pointer
 * @param size Size to copy from source
*/
HOGL_API void hogl_memcpy(void* dst, const void* src, size_t size);

/**
 * @brief Frees the memory specified by p
 * @param p Memory to free
*/
HOGL_API void hogl_free(void* p);

/**
 * @brief Returns the allocated byte count, if hogl_free was called allocation size is decremented by pointer size
*/
HOGL_API int hogl_get_allocated_bytes(void);

/**
 * @brief Returns the total number of allocations, if hogl_free was called allocation count is decremented by 1
*/
HOGL_API int hogl_get_allocations(void);

#endif