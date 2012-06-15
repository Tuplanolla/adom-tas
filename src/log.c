/**
Logs events.
**/
#ifndef LOG_C
#define LOG_C

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include "problem.h"
#include "log.h"

FILE * error_stream;
FILE * warning_stream;
FILE * note_stream;
FILE * call_stream;

const char * const separator = " - ";
const char * const error_str = "Error";
const char * const warning_str = "Warning";
const char * const note_str = "Note";
const char * const call_str = "Call";

/**
Formats and logs a message.

@param stream The destination stream.
@param fmt The message format.
@param ap The parameters to format.
@return The amount of characters written.
**/
int vfprintfl(FILE * stream, const char * fmt, va_list ap) {
	/*
	Creativity is required since <code>time</code> and <code>localtime</code> are unavailable.
	*/
	int result = 0;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	const time_t timep = tv.tv_sec;
	struct tm tm;
	localtime_r(&timep, &tm);
	result += fprintf(stream, "%02u:%02u:%02u%s#%u%s", tm.tm_hour, tm.tm_min, tm.tm_sec, separator, getpid(), separator);
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
	if (error_stream != NULL) {
		fprintfl(error_stream, "%s%s%s", error_str, separator, problem_message(code));
	}
	return code;
}

/**
Logs a warning message and returns its error code.

@param code The error code.
@return The error code.
**/
problem_t warning(const problem_t code) {
	if (warning_stream != NULL) {
		fprintfl(warning_stream, "%s%s%s", warning_str, separator, problem_message(code));
	}
	return code;
}

/**
Logs a note message and returns its error code.

@param code The error code.
@return The error code.
**/
problem_t note(const problem_t code) {
	if (note_stream != NULL) {
		fprintfl(note_stream, "%s%s%s", note_str, separator, problem_message(code));
	}
	return code;
}

/**
Logs a call and returns its error code.

@param fmt The function name.
@param ... The function parameters.
@return The error code.
**/
problem_t call(const char * fmt, ...) {
	if (call_stream != NULL) {
		va_list	ap;
		va_start(ap, fmt);
		const size_t size = strlen(call_str)+strlen(separator)+strlen(fmt)+1;
		char * call_fmt = malloc(size);
		snprintf(call_fmt, size, "%s%s%s", call_str, separator, fmt);
		vfprintfl(call_stream, call_fmt, ap);
		free(call_fmt);
		va_end(ap);
	}
	return NO_PROBLEM;
}

#endif
