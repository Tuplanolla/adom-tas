/**
@see gui.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef GUI_H
#define GUI_H

#include <curses.h>//WINDOW

int gui_uninit(void);
int gui_init(void);
int gui_draw(WINDOW * win);

int gui_draw_overlay(WINDOW * win);//TODO remove

#endif
