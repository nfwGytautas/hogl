#include "hogl_log.h"

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

#include "hogl_core/os/hogl_os.h"

#define MESSAGE_SIZE 500

static hogl_log_message_cb s_callback = NULL;

void hogl_log_impl(const char* color, const char* type, int line, const char* function, const char* format, ...)
{
	struct timespec ts;
	struct tm* tm;
	va_list args;
	time_t t;
	char msgBuff[MESSAGE_SIZE];
	int offset = 0;
	int writeSize = 0;
	int endSize = strlen("\x1b[0m");

	// Color, type, thread id
	writeSize = sprintf(msgBuff, "%s[%8s MESSAGE]\n\tTHREAD  : %ld\n\tDATE    : ", color, type, hogl_get_thread_id());
	if (writeSize < 0) {
		return;
	}
	offset += writeSize;

	// Date
	t = time(NULL);
	tm = localtime(&t);

	writeSize = strftime(msgBuff + offset, MESSAGE_SIZE - (size_t)offset, "%Y-%m-%d %H:%M:%S", tm);
	if (writeSize < 0) {
		return;
	}
	offset += writeSize;

	if (timespec_get(&ts, TIME_UTC) != TIME_UTC) {
		// timespec_get error
		writeSize = sprintf(msgBuff + offset, ".xxx\n\tFUNCTION: %s\n\tLINE    : %d\n\tMESSAGE : ", function, line);
	}
	else {
		writeSize = sprintf(msgBuff + offset, ".%03ld\n\tFUNCTION: %s\n\tLINE    : %d\n\tMESSAGE : ", ts.tv_nsec / 1000000, function, line);
	}

	if (writeSize < 0) {
		return;
	}
	offset += writeSize;

	// Message
	va_start(args, format);
	writeSize = vsprintf(msgBuff + offset, format, args);
	va_end(args);

	if (writeSize < 0) {
		return;
	}
	offset += writeSize;

	// End
	strncat(msgBuff + offset, "\x1b[0m", endSize);
	offset += endSize;

	// Printing
	if (s_callback == NULL) {
		printf("%s\n", msgBuff);
	}
	else {
		s_callback(msgBuff, offset);
	}
}

void hogl_set_log_cb(hogl_log_message_cb callback)
{
	s_callback = callback;
}
