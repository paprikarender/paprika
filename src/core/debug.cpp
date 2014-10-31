#include "debug.hpp"
#include <stdarg.h>
#include <stdio.h>
#include <string>

namespace paprika {
namespace core {

// Error Reporting Definitions
#define PAPRIKA_ERROR_IGNORE 0
#define PAPRIKA_ERROR_CONTINUE 1
#define PAPRIKA_ERROR_ABORT 2

// Error Reporting Functions
static void processError(const char *format, va_list args, const char *message, int disposition)
{
	char errorBuf[2048];
	vsnprintf(errorBuf, sizeof(errorBuf), format, args);

	switch (disposition)
	{
		case PAPRIKA_ERROR_IGNORE:
			return;
		case PAPRIKA_ERROR_CONTINUE:
			fprintf(stderr, "%s: %s\n", message, errorBuf);
			break;
		case PAPRIKA_ERROR_ABORT:
			fprintf(stderr, "%s: %s\n", message, errorBuf);
			exit(1);
	}
}

LIBPAPRIKA_EXPORT void Info(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	processError(format, args, "Notice", PAPRIKA_ERROR_CONTINUE);
	va_end(args);
}

LIBPAPRIKA_EXPORT void Warning(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	processError(format, args, "Warning", PAPRIKA_ERROR_CONTINUE);
	va_end(args);
}

LIBPAPRIKA_EXPORT void Error(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	processError(format, args, "Error", PAPRIKA_ERROR_CONTINUE);
	va_end(args);
}

LIBPAPRIKA_EXPORT void Severe(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	processError(format, args, "Fatal Error", PAPRIKA_ERROR_ABORT);
	va_end(args);
}

}		/* util */
}		/* paprika */
