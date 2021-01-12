#include "hogl_os.h"

#ifdef _WIN32

#include <Windows.h>

unsigned long hogl_get_thread_id(void) {
	return GetCurrentThreadId();
}

int hogl_atomic_set_b32(int* destination, int newValue) {
	return InterlockedExchange(destination, newValue);
}

int hogl_atomic_get_b32(int* variable) {
	return InterlockedExchangeAdd(variable, 0);
}

int hogl_atomic_add_b32(int* destination, int value) {
	return InterlockedExchangeAdd(destination, value);
}

int hogl_atomic_substract_b32(int* variable, int value) {
	return InterlockedExchangeAdd(variable, value * -1);
}

#elif __linux__

// NOT YET IMPLEMENTED

#else

// NOT YET IMPLEMENTED

#endif
