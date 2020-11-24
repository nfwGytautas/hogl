#include "hogl_log.h"

#include <stdio.h>
#include <time.h>

#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <sys/types.h>
#endif

// Thread id
unsigned long get_thread_id() {
#ifdef _WIN32
	return GetCurrentThreadId();
#elif __linux__
	return gettid();
#else
	return 0;
#endif
}

#define _print_tttn(color, type, line, func, msg) \
	struct tm* tm;\
	time_t t;\
	char date[30];\
\
	t = time(NULL);\
	tm = localtime(&t);\
\
	strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", tm);\
\
	struct timespec ts;\
	if (timespec_get(&ts, TIME_UTC) != TIME_UTC) {\
		/* timespec_get get error */\
		printf("%s[%8s on thread: %ld at %s.xxx in function '%s' on line: %d]: %s \x1b[0m\n", color, type, get_thread_id(), date, func, line, msg);\
	}\
	else {\
		printf("%s[%8s on thread: %ld at %s.%3ld in function '%s' on line: %d]: %s \x1b[0m\n", color, type, get_thread_id(), date, ts.tv_nsec / 1000000, func, line, msg);\
	}

#define RED "\x1b[31m"
#define YELLOW "\x1b[33m"
#define CYAN "\x1b[36m"
#define GRAY "\x1b[90m"

void _hogl_log_error(int line, const char* function, const char* msg)
{
	_print_tttn(RED, "ERROR", line, function, msg);
}

void _hogl_log_warn(int line, const char* function, const char* msg)
{
	_print_tttn(YELLOW, "WARNING", line, function, msg);
}

void _hogl_log_info(int line, const char* function, const char* msg)
{
	_print_tttn(CYAN, "INFO", line, function, msg);
}

void _hogl_log_trace(int line, const char* function, const char* msg)
{
	_print_tttn(GRAY, "TRACE", line, function, msg);
}
