#include "hogl_memory.h"

#include <stdlib.h>

#include "hogl_core/hogl_log.h"
#include "hogl_core/os/hogl_os.h"

#ifndef HOGL_DISABLE_MEM_TRACK

#ifdef _WIN32

static int s_allocated = 0;
static int s_allocations = 0;

void* hogl_malloc(unsigned int size)
{
	hogl_log_trace("Allocating %ld bytes", size);
	hogl_atomic_add_b32(&s_allocated, size);
	hogl_atomic_add_b32(&s_allocations, 1);

	// Using the default libc malloc for now
	return malloc(size);
}

void hogl_free(void* p)
{
	int blockSize = _msize(p);
	hogl_log_trace("Freeing %ld bytes", blockSize);
	hogl_atomic_substract_b32(&s_allocated, blockSize);
	hogl_atomic_substract_b32(&s_allocations, 1);

	// Using the default libc free for now
	free(p);
}

int hogl_get_allocated_bytes(void)
{
	return hogl_atomic_get_b32(&s_allocated);
}

int hogl_get_allocations(void)
{
	return hogl_atomic_get_b32(&s_allocations);
}

void hogl_print_allocated(void)
{
	//hogl_log_info();
	hogl_log_info("hogl memory allocated: %d", hogl_atomic_get_b32(&s_allocated));
}

#endif
#endif
