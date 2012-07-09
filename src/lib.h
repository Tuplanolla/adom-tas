/**
@see lib.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef LIB_H
#define LIB_H

#include <stddef.h>//size_t
#include <time.h>//time_t

#include <curses.h>//WINDOW

#include "gnu.h"//__*__
#include "problem.h"//problem_t

typedef int (* printf_f)(const char * format, ...);
typedef int (* unlink_f)(const char * path);
typedef int (* ioctl_f)(int d, unsigned long request, ...);
typedef time_t (* time_f)(time_t * timer);
typedef struct tm * (* localtime_f)(const time_t * timep);
typedef void (* srandom_f)(unsigned int seed);
typedef long (* random_f)(void);
typedef int (* init_pair_f)(short pair, short f, short b);
typedef int (* wrefresh_f)(WINDOW * win);
typedef int (* wgetch_f)(WINDOW * win);
typedef int (* endwin_f)(void);

extern printf_f um_printf;
extern unlink_f um_unlink;
extern ioctl_f um_ioctl;
extern time_f um_time;
extern localtime_f um_localtime;
extern srandom_f um_srandom;
extern random_f um_random;
extern init_pair_f um_init_pair;
extern wrefresh_f um_wrefresh;
extern wgetch_f um_wgetch;
extern endwin_f um_endwin;

extern unsigned int current_state;
extern unsigned char dur;
extern char name[77];
extern int surplus_turns;
extern int previous_turns;

problem_t init_lib(void);
problem_t uninit_lib(void);
void save_quit_load(void);

#endif
