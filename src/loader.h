/**
Lists the functions of the loader.
**/
#ifndef LOADER_H
#define LOADER_H

#include <curses.h>

/**
Declares the original dynamically linked library functions.
**/
typedef int (* INIT_PAIR)(short pair, short f, short b);
typedef int (* WCLEAR)(WINDOW * win);//I really want to overload all these?
typedef int (* WREFRESH)(WINDOW * win);
typedef int (* WMOVE)(WINDOW * win, int y, int x);
typedef int (* WADDCH)(WINDOW * win, chtype ch);
typedef int (* WADDNSTR)(WINDOW * win, const char * str, int n);
typedef chtype (*WINCH)(WINDOW * win);
typedef int (* WGETCH)(WINDOW * win);
typedef int (* WGETNSTR)(WINDOW * win, char * str, int n);
typedef int (* UNLINK)(const char * pathname);
typedef time_t (* TIME)(time_t * timer);
typedef int (* IOCTL)(int d, unsigned long request, ...);
typedef struct tm * (* LOCALTIME)(const time_t * timep);
typedef long (* RANDOM)();
typedef void (* SRANDOM)(unsigned int seed);

extern INIT_PAIR real_init_pair;
extern WCLEAR real_wclear;
extern WREFRESH real_wrefresh; 
extern WMOVE real_wmove;
extern WADDCH real_waddch;
extern WADDNSTR real_waddnstr;
extern WINCH real_winch;
extern WGETCH real_wgetch;
extern WGETNSTR real_wgetnstr;
extern UNLINK real_unlink;
extern TIME real_time;
extern LOCALTIME real_localtime;
extern RANDOM real_random;
extern SRANDOM real_srandom;
extern IOCTL real_ioctl;

#endif
