/**
@see interface.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef INTERFACE_H
#define INTERFACE_H

#include <curses.h>

#include "util.h"//bool
#include "problem.h"//problem_t

extern bool quitting;
extern bool inactive;
extern bool condensed;
extern bool hidden;
extern bool playing;
extern bool paused;

problem_t init_interface(void);
problem_t draw_interface(WINDOW * win);

#endif
