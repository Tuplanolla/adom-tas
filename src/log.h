/**
@file log.h
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef LOG_H
#define LOG_H

#include <stdarg.h>//va_*
#include <stdio.h>//FILE

#include "gnu.h"//__*__
#include "prob.h"//problem_d

int log_vfprintf(FILE * stream, const char * format, va_list ap) __attribute__ ((format(printf, 2, 0)));
int log_fprintf(FILE * stream, const char * format, ...) __attribute__ ((format(printf, 2, 3)));
problem_d log_error(problem_d code);
problem_d log_warning(problem_d code);
problem_d log_notice(problem_d code);
int log_call(const char * format, ...) __attribute__ ((format(printf, 1, 2)));

#endif
