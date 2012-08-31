/**
Logs messages.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#include <stdarg.h>//va_*
#include <stdlib.h>//*alloc, free, NULL
#include <stdio.h>//*print*, *flush, FILE
#include <string.h>//str*
#include <sys/time.h>//gettimeofday, struct timeval
#include <unistd.h>//getpid

#include "prob.h"//problem_message, probno, *_PROBLEM
#include "def.h"//log_*
#include "cfg.h"//cfg_*

#include "log.h"

/**
Formats and logs a message.

@param stream The destination stream.
@param format The message format.
@param ap The parameters to format.
@return The amount of characters written.
**/
int log_vfprintf(FILE * const stream, const char * const format, va_list ap) {
	/*
	Creativity is required since
	 <code>time</code>,
	 <code>localtime</code> and
	 <code>printf</code>
	  are unavailable.
	*/
	int result = 0;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	const time_t timep = (time_t )tv.tv_sec;
	struct tm tm;
	localtime_r(&timep, &tm);
	result += fprintf(stream, "%02u:%02u:%02u%s#%u%s",
			tm.tm_hour, tm.tm_min, tm.tm_sec,
			def_log_separator, getpid(), def_log_separator);
	result += vfprintf(stream, format, ap);
	result += fprintf(stream, "\n");
	fflush(stream);
	return result;
}

/**
Formats and logs a message.

@param stream The destination stream.
@param format The message format.
@param ... The parameters to format.
@return The amount of characters written.
**/
int log_fprintf(FILE * const stream, const char * const format, ...) {
	va_list ap;
	va_start(ap, format);
	const int result = log_vfprintf(stream, format, ap);
	va_end(ap);
	return result;
}

/**
Logs an error message and returns its error code.

@param code The error code.
@return The error code.
**/
problem_d log_error(const problem_d code) {
	if (cfg_error_stream != NULL) {
		log_fprintf(cfg_error_stream, "%s%s%s",
				def_log_error, def_log_separator, problem_message(code));
	}
	return code;
}
/**
Logs a warning message and returns its error code.

@param code The error code.
@return The error code.
**/
problem_d log_warning(const problem_d code) {
	if (cfg_warning_stream != NULL) {
		log_fprintf(cfg_warning_stream, "%s%s%s",
				def_log_warning, def_log_separator, problem_message(code));
	}
	return code;
}
/**
Logs a notice message and returns its error code.

@param code The error code.
@return The error code.
**/
problem_d log_notice(const problem_d code) {
	if (cfg_notice_stream != NULL) {
		log_fprintf(cfg_notice_stream, "%s%s%s",
				def_log_notice, def_log_separator, problem_message(code));
	}
	return code;
}

/**
Logs a call and returns its error code.

@param format The function name.
@param ... The function parameters.
@return The error code.
**/
int log_call(const char * const format, ...) {
	int result = 0;
	if (cfg_call_stream != NULL) {
		va_list	ap;
		va_start(ap, format);
		const size_t size = strlen(def_log_call)
				+ strlen(def_log_separator)
				+ strlen(format) + 1;
		char * const call_fmt = malloc(size);
		if (call_fmt == NULL) {
			probno = log_error(MALLOC_PROBLEM);
			result = -1;
		}
		else {
			snprintf(call_fmt, size, "%s%s%s",
					def_log_call, def_log_separator, format);
			log_vfprintf(cfg_call_stream, call_fmt, ap);
			free(call_fmt);
		}
		va_end(ap);
	}
	return result;
}
