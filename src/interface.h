/**
@see gui.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef INTERFACE_H
#define INTERFACE_H

#include <curses.h>

#include "util.h"
#include "problem.h"

extern bool quitting;
extern bool inactive;
extern bool condensed;
extern bool hidden;
extern bool playing;
extern bool paused;

problem_t init_interface(void);
void draw_interface(WINDOW * win);

#endif
