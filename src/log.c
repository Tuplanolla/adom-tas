/**
Provides logging.
**/
#ifndef LOG_C
#define LOG_C

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "problem.h"

FILE * error_log;
FILE * warning_log;
FILE * note_log;
FILE * call_log;
FILE * input;
FILE * output;

/**
Formats and logs a message.

@param stream The destination stream.
@param fmt The message format.
@param ap The parameters to format.
@return The amount of characters written.
**/
int vfprintfl(FILE * stream, const char * fmt, va_list ap) {
	int result = 0;
	const time_t timep = time(NULL);
	struct tm * tm = localtime(&timep);
	result += fprintf(stream, "[%d:%d:%d] ", tm->tm_hour, tm->tm_min, tm->tm_sec);
	result += vfprintf(stream, fmt, ap);
	result += fprintf(stream, "\n");
	fflush(stream);
	return result;
}

/**
Formats and logs a message.

@param stream The destination stream.
@param fmt The message format.
@param ... The parameters to format.
@return The amount of characters written.
**/
int fprintfl(FILE * stream, const char * fmt, ...) {
	va_list	ap;
	va_start(ap, fmt);
	const int result = vfprintfl(stream, fmt, ap);
	va_end(ap);
	return result;
}

/**
Logs an error message and returns its error code.

@param code The error code.
@return The error code.
**/
problem_t error(const problem_t code) {
	if (error_log != NULL) {
		fprintfl(error_log, "Error: %s", problem_message(code));
	}
	return code;
}

/**
Logs a warning message and returns its error code.

@param code The error code.
@return The error code.
**/
problem_t warning(const problem_t code) {
	if (warning_log != NULL) {
		fprintfl(warning_log, "Warning: %s", problem_message(code));
	}
	return code;
}

/**
Logs a note message and returns its error code.

@param code The error code.
@return The error code.
**/
problem_t note(const problem_t code) {
	if (note_log != NULL) {
		fprintfl(note_log, "Note: %s", problem_message(code));
	}
	return code;
}

#endif
