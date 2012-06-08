/**
Provides logs.
**/
#ifndef LOG_H
#define LOG_H

extern FILE * error_stream;
extern FILE * warning_stream;
extern FILE * note_stream;
extern FILE * call_stream;

int vfprintfl(FILE * stream, const char * fmt, va_list ap);
int fprintfl(FILE * stream, const char * fmt, ...);
problem_t error(const problem_t code);
problem_t warning(const problem_t code);
problem_t note(const problem_t code);
problem_t call(const char * fmt, ...);

#endif
