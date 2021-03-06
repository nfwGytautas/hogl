#include "hogl_memory.h"

#include <stdlib.h>
#include <string.h>

#include "hogl_core/shared/hogl_log.h"
#include "hogl_core/os/hogl_os.h"

#ifndef HOGL_DISABLE_MEM_TRACK

#ifdef _WIN32

// WINDOWS

static int s_allocated = 0;
static int s_allocations = 0;

void* hogl_malloc(unsigned int size) {
	if (size <= 0) {
		hogl_log_error("Tried to allocate bad memory size (%ld) returning NULL", size);
		return NULL;
	}

	hogl_log_trace("Allocating %ld bytes", size);
	hogl_atomic_add_b32(&s_allocated, size);
	hogl_atomic_add_b32(&s_allocations, 1);

	// Using the default libc malloc for now
	return malloc(size);
}

void hogl_memcpy(void* dst, const void* src, size_t size) {
	size_t dst_size = _msize(dst);
	if (dst_size < size) {
		hogl_log_error("Copying %ld length data to a pointer that only has %ld", size, dst_size);
	}

	memcpy(dst, src, size);
}

void hogl_smemcpy(void* dst, const void* src, size_t size) {
	memcpy(dst, src, size);
}

void* hogl_realloc(void* dst, size_t new_size) {
	void* new_ptr = realloc(dst, new_size);

	if (new_ptr == NULL) {
		hogl_log_warn("Failed to realloc a pointer");
	}

	return new_ptr;
}

void hogl_memset(void* dst, int value, size_t count) {
	memset(dst, value, count);
}

void hogl_free(void* p) {
	size_t blockSize = _msize(p);
	hogl_log_trace("Freeing %ld bytes", blockSize);
	hogl_atomic_substract_b32(&s_allocated, blockSize);
	hogl_atomic_substract_b32(&s_allocations, 1);

	// Using the default libc free for now
	free(p);
}

int hogl_get_allocated_bytes(void) {
	return hogl_atomic_get_b32(&s_allocated);
}

int hogl_get_allocations(void) {
	return hogl_atomic_get_b32(&s_allocations);
}

void hogl_print_allocated(void) {
	//hogl_log_info();
	hogl_log_info("hogl memory allocated: %d", hogl_atomic_get_b32(&s_allocated));
}

#elif __linux__

// LINUX

// NOT YET IMPLEMENTED

#else

// OTHER
// NOT YET IMPLEMENTED

#endif

#else

// DEFAULT



#endif