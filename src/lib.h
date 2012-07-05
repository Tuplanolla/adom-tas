/**
Provides reading for the whole family.
**/
#ifndef LIB_H
#define LIB_H

#include <stddef.h>//size_t
#include <time.h>//time_t

#include <curses.h>//WINDOW

#include "gnu.h"//__*__

typedef int (* UNLINK)(const char * path);
typedef int (* IOCTL)(int d, unsigned long request, ...);
typedef time_t (* TIME)(time_t * timer);
typedef struct tm * (* LOCALTIME)(const time_t * timep);
typedef void (* SRANDOM)(unsigned int seed);
typedef long (* RANDOM)(void);
typedef int (* INIT_PAIR)(short pair, short f, short b);
typedef int (* WREFRESH)(WINDOW * win);
typedef int (* WGETCH)(WINDOW * win);
typedef int (* EXIT)(int status) __attribute__ ((__noreturn__));

extern UNLINK um_unlink;
extern IOCTL um_ioctl;
extern TIME um_time;
extern LOCALTIME um_localtime;
extern SRANDOM um_srandom;
extern RANDOM um_random;
extern INIT_PAIR um_init_pair;
extern WREFRESH um_wrefresh;
extern WGETCH um_wgetch;
extern EXIT um_exit;

void injector(void);

#endif
