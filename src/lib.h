/**
Provides reading for the whole family.
**/
#ifndef LIB_H
#define LIB_H

typedef int (* UNLINK)(const char * path);
typedef int (* IOCTL)(int d, unsigned long request, ...);
typedef time_t (* TIME)(time_t * timer);
typedef struct tm * (* LOCALTIME)(const time_t * timep);
typedef void (* SRANDOM)(unsigned int seed);
typedef long (* RANDOM)();
typedef int (* INIT_PAIR)(short pair, short f, short b);
typedef int (* WREFRESH)(WINDOW * win);
typedef int (* WGETCH)(WINDOW * win);

extern UNLINK um_unlink;
extern IOCTL um_ioctl;
extern TIME um_time;
extern LOCALTIME um_localtime;
extern SRANDOM um_srandom;
extern RANDOM um_random;
extern INIT_PAIR um_init_pair;
extern WREFRESH um_wrefresh;
extern WGETCH um_wgetch;

#endif
