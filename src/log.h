/**
Logs events.

Errors are major problems.
Warnings are minor problems.
Notes are not problems.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef LOG_H
#define LOG_H

#include <stdarg.h>//va_list
#include <stdio.h>//FILE

#include "gnu.h"//__*__
#include "problem.h"//problem_t

int vfprintfl(FILE * stream, const char * fmt, va_list ap) __attribute__((format(printf, 2, 0)));
int fprintfl(FILE * stream, const char * fmt, ...) __attribute__((format(printf, 2, 3)));
problem_t error(problem_t code);
problem_t warning(problem_t code);
problem_t note(problem_t code);
problem_t call(const char * fmt, ...) __attribute__((format(printf, 1, 2)));

#endif
