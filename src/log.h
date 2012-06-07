/**
Provides logging.
**/
#ifndef LOG_H
#define LOG_H

extern FILE * error_log;
extern FILE * warning_log;
extern FILE * note_log;
extern FILE * call_log;
extern FILE * input;
extern FILE * output;

int problem(const int code);//temporary

int vfprintfl(FILE * stream, const char * fmt, va_list ap);
int fprintfl(FILE * stream, const char * fmt, ...);
//int fwritel(FILE * stream, size_t size, const unsigned char * ptr);
problem_t error(const problem_t code);
problem_t warning(const problem_t code);
problem_t note(const problem_t code);

#endif
