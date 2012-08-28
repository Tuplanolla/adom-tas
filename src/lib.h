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
#include "util.h"//bool
#include "prob.h"//problem_d

/**
The execution modes.
**/
enum imode_e {
	IN_GAME      = 0x0001,
	INITIALIZING = 0x0002,
	INITIALIZED  = 0x0004,
	INACTIVE     = 0x0008,
	MENUINFO     = 0x0010,
	CONDENSED    = 0x0020,
	HIDDEN       = 0x0040,
	PLAYING      = 0x0080,
	PAUSED       = 0x0100,
	QUITTING     = 0x0200,
	ROLLING      = 0x0400,
	AHH          = 0x0800
};
typedef enum imode_e imode_d;

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

extern imode_d imode;

extern unsigned int previous_frames;
extern int current_save;
extern unsigned short int current_duration;
extern int previous_inputs[4];

/**
Ensures overloaded functions aren't used outside this module.
**/
#ifndef UM_ALIAS

//#define printf um_printf
#define unlink um_unlink
#define ioctl um_ioctl
#define time um_time
#define localtime um_localtime
#define srandom um_srandom
#define random um_random
#define wrefresh um_wrefresh
#define init_pair um_init_pair
#define waddnstr um_waddnstr
#define wgetch um_wgetch
#define endwin um_endwin

#endif

#endif
