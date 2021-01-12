/**
* @brief hogl memory file contains functionality and types used for working with memory, 
* all function will map to their standard operating system function if memory tracking is disabled
* which it should be if building for the final release, memory tracking is a debugging tool. For 
* disabling of memory tracking look at hogl define flags. Some operating system don't have memory
* tracking functionality.
*/

#ifndef _HOGL_MEMORY_
#define _HOGL_MEMORY_

#include "hogl_core/shared/hogl_def.h"

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
 * @brief Copies data from src to dst where dst is a stack allocated variable, basically skips 
 * the automatic bound checking, this will be the same as memcpy if HOGL_DISABLE_MEM_TRACK is defined
 * @param dst Destination pointer
 * @param src Source pointer
 * @param size Size to copy from source
*/
HOGL_API void hogl_smemcpy(void* dst, const void* src, size_t size);

/**
 * @brief Expands the specified data pointer to match the new size, the existing data is preserved up to the
 * previous size, the resulting pointer can be different from the original, depending on the implementation
 * @param dst The pointer to reallocate
 * @param new_size New size of the pointer
 * @return Pointer to the new address of the memory region or NULL if the call failed
*/
HOGL_API void* hogl_realloc(void* dst, size_t new_size);

/**
 * @brief Sets the memory of the specified pointer to the value
 * @param dst Pointer to set
 * @param value Value to fill the pointer with
 * @param count Number of elements to set
*/
HOGL_API void hogl_memset(void* dst, int value, size_t count);

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