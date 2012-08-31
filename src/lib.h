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
Uh.
**/
enum progress_e {
	MAIN,
	PRINTF,
	WADDNSTR,
	EXIT
};
typedef enum progress_e progress_d;

/**
Uhh.
**/
struct options_s {
	bool key_active;
	bool progress_active;
	progress_d progress;
	bool record_active;
	bool record_paused;
	bool gui_active;
	bool gui_menu_active;
	bool gui_info_active;
	bool gui_overlay_active;
	bool gui_condensed;
	bool gui_hidden;
	bool roll_active;
};
typedef struct options_s options_d;

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

extern options_d options;

extern unsigned int previous_frames;
extern int current_save;
extern unsigned short int current_duration;
extern int previous_inputs[4];

/**
Ensures overloaded functions aren't used outside this module.
**/
#ifndef UM_ALIAS

//#define printf um_printf//TODO fix conflict with GNU stuff
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
