/**
@see log.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef LOG_H
#define LOG_H

#include <stdarg.h>//va_*
#include <stdio.h>//FILE

#include "gnu.h"//__*__
#include "prob.h"//problem_d

int vfprintfl(FILE * stream, const char * fmt, va_list ap) __attribute__ ((format(printf, 2, 0)));
int fprintfl(FILE * stream, const char * fmt, ...) __attribute__ ((format(printf, 2, 3)));
problem_d error(problem_d code);
problem_d warning(problem_d code);
problem_d note(problem_d code);
problem_d call(const char * fmt, ...) __attribute__ ((format(printf, 1, 2)));

#endif
