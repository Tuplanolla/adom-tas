/**
Provides.
**/
#ifndef __H
#define __H

/**
Lists error codes.
**/
enum error_e {
	NO_ERROR,
	SETENV_ERROR,
	USER_ID_ERROR,
	DLOPEN_ERROR,
	STAT_ERROR,
	WRONG_VERSION_ERROR,
	WRONG_SIZE_ERROR,
};
typedef enum error_e error_t;

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
typedef int (*SPRINTF)(char *str, const char *format, ...);
typedef int (*UNLINK)(const char *pathname);
typedef time_t (*TIME)(time_t *timer);
typedef int (*IOCTL)(int d, unsigned long request, ...);

#endif
