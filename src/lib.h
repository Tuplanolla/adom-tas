/**
@file lib.h
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef LIB_H
#define LIB_H

#include <stddef.h>//size_t
#include <time.h>//time_t

#include <curses.h>//WINDOW

#include "gnu.h"//__*__
#include "util.h"//bool

/**
Lists the functions that are progressed.
**/
enum progress_e {
	MAIN,
	PRINTF,
	WADDNSTR,
	EXIT
};
typedef enum progress_e progress_d;

/**
Contains various options.
**/
struct options_s {
	progress_d progress;
	bool k_on;//used to skip no-time refreshes
	bool play_on;
	bool play_paused;
	bool gui_menu;
	bool gui_info;
	bool gui_overlay;
	bool gui_condensed;
	bool gui_hidden;
	bool roll_on;
	bool roll_cataloged;
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

extern printf_f orig_printf;
extern unlink_f orig_unlink;
extern ioctl_f orig_ioctl;
extern time_f orig_time;
extern localtime_f orig_localtime;
extern srandom_f orig_srandom;
extern random_f orig_random;
extern wrefresh_f orig_wrefresh;
extern init_pair_f orig_init_pair;
extern waddnstr_f orig_waddnstr;
extern wgetch_f orig_wgetch;
extern endwin_f orig_endwin;

extern options_d options;

extern unsigned int previous_record_frames;
extern int current_save;
extern unsigned short int current_duration;
extern int previous_inputs[4];

#endif
