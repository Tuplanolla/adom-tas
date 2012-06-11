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

#include "problem.h"
#include "log.h"

FILE * error_stream;
FILE * warning_stream;
FILE * note_stream;
FILE * call_stream;

const char * const problem_fmt = "%s: %s";
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
	result += fprintf(stream, "%02d:%02d:%02d - ", tm.tm_hour, tm.tm_min, tm.tm_sec);
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
		fprintfl(error_stream, problem_fmt, error_str, problem_message(code));
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
		fprintfl(warning_stream, problem_fmt, warning_str, problem_message(code));
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
		fprintfl(note_stream, problem_fmt, note_str, problem_message(code));
	}
	return code;
}

/**
Logs a call.

@param fmt The function name.
@param ... The function parameters.
**/
problem_t call(const char * fmt, ...) {
	if (call_stream != NULL) {
		va_list	ap;
		va_start(ap, fmt);
		size_t size = strlen(call_str)+strlen(problem_fmt)+strlen(fmt)+1;
		char * call_fmt = malloc(size);
		snprintf(call_fmt, size, problem_fmt, call_str, fmt);
		vfprintfl(call_stream, call_fmt, ap);
		free(call_fmt);
		va_end(ap);
	}
	return NO_PROBLEM;
}

#endif
