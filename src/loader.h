/**
Lists the functions of the loader.
**/
#ifndef LOADER_H
#define LOADER_H

/**
Declares the original dynamically linked library functions.
**/
typedef int (*INIT_PAIR)(short pair, short f, short b);
typedef int (*WCLEAR)(WINDOW *win);//I really want to overload all these?
typedef int (*WREFRESH)(WINDOW *win);
typedef int (*WMOVE)(WINDOW *win, int y, int x);
typedef int (*WADDCH)(WINDOW *win, chtype ch);
typedef int (*WADDNSTR)(WINDOW *win, const char *str, int n);
typedef chtype (*WINCH)(WINDOW *win);
typedef int (*WGETCH)(WINDOW *win);
typedef int (*WGETNSTR)(WINDOW *win, char *str, int n);
typedef int (*UNLINK)(const char *pathname);
typedef time_t (*TIME)(time_t *timer);
typedef int (*IOCTL)(int d, unsigned long request, ...);
typedef int (*RAND)();
typedef void (*SRAND)(unsigned int seed);
typedef long (*RANDOM)();
typedef void (*SRANDOM)(unsigned int seed);

#endif
