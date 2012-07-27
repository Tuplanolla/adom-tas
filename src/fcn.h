/**
@see fcn.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef FCN_H
#define FCN_H

#include <stddef.h>//size_t
#include <time.h>//time_t

#include <curses.h>//WINDOW

#include "gnu.h"//__*__
#include "prob.h"//problem_t

typedef int (* printf_f)(const char * format, ...);
typedef int (* unlink_f)(const char * path);
typedef int (* ioctl_f)(int d, unsigned long request, ...);
typedef time_t (* time_f)(time_t * timer);
typedef struct tm * (* localtime_f)(const time_t * timep);
typedef void (* srandom_f)(unsigned int seed);
typedef long (* random_f)(void);
typedef int (* wrefresh_f)(WINDOW * win);
typedef int (* init_pair_f)(short pair, short f, short b);
typedef int (* waddnstr_f)(WINDOW * win, const char * str, int n);
typedef int (* wgetch_f)(WINDOW * win);
typedef int (* endwin_f)(void);

extern printf_f um_printf;
extern unlink_f um_unlink;
extern ioctl_f um_ioctl;
extern time_f um_time;
extern localtime_f um_localtime;
extern srandom_f um_srandom;
extern random_f um_random;
extern wrefresh_f um_wrefresh;
extern init_pair_f um_init_pair;
extern waddnstr_f um_waddnstr;
extern wgetch_f um_wgetch;
extern endwin_f um_endwin;

extern int current_state;
extern unsigned char dur;
extern char previous_inputs[77];
extern int surplus_turns;
extern int previous_turns;

problem_t init_fcn(void);
problem_t uninit_fcn(void);
void save_quit_load(void);

#endif
