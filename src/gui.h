/**
@see gui.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef GUI_H
#define GUI_H

#include <curses.h>//WINDOW

#include "prob.h"//problem_d

problem_d uninit_gui(void);
problem_d init_gui(void);
problem_d draw_gui(WINDOW * win);
problem_d draw_overlay(WINDOW * win);

#endif
