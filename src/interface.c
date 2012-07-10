/**
Serves as a template.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef INTERFACE_C
#define INTERFACE_C

#include <stdlib.h>

#include <curses.h>

#include "util.h"
#include "exec.h"
#include "shm.h"
#include "problem.h"//problem_t, *_PROBLEM
#include "record.h"
#include "def.h"
#include "loader.h"
#include "lib.h"
#include "log.h"
#include "config.h"

#include "interface.h"

/**
Modes.
**/
intern bool quitting = FALSE;
intern bool inactive = FALSE;
intern bool condensed = FALSE;
intern bool hidden = FALSE;
intern bool playing = FALSE;
intern bool paused = FALSE;
unsigned int previous_count = 0;
char * previous_codes;
frame_t * frame = NULL;

/**
The status bar window.
**/
WINDOW * status_win = NULL;

/**
The save state menu window.
**/
WINDOW * menu_win = NULL;

/**
The save state list subwindow.
**/
WINDOW * menu_states_win = NULL;

/**
The save state preview subwindow.
**/
WINDOW * menu_chs_win = NULL;

problem_t init_interface(void) {
	/*
	Initializes the custom color pairs.
	*/
	const size_t colors = sizeof interface_colors / sizeof *interface_colors;
	for (size_t color = 1; color < colors; color++) {
		init_pair(pairs + color, COLOR_BLACK, interface_colors[color]);
	}

	/*
	Creates the windows.
	*/
	status_win = newwin(1, cols, rows - 1, 0);
	const int nlines = rows / 2;
	const int ncols = cols / 2;
	const int begin_y = rows / 4 - 1;
	const int begin_x = cols / 4;
	menu_win = newwin(1 + nlines + 1, 1 + ncols + 1, begin_y - 1, begin_x - 1);
	menu_states_win = newwin(3, 1 + ncols + 1, begin_y + nlines, begin_x - 1);
	menu_chs_win = newwin(nlines, ncols, begin_y, begin_x);

	return NO_PROBLEM;
}

problem_t uninit_interface(void) {
	delwin(status_win);
	delwin(menu_win);
	delwin(menu_chs_win);
	delwin(menu_states_win);

	return NO_PROBLEM;
}

/**
Draws the status bar in the bottom of a window from right to left.

@param win The window.
**/
problem_t draw_status(void) {
	int x = cols;
	short pair = 0;
	char * const str = malloc(cols);
	if (str == NULL) {
		return error(MALLOC_PROBLEM);
	}
	#define wrefresh_ADDSTR(format, ...) do {\
				snprintf(str, cols, format, __VA_ARGS__);\
				x -= strlen(str);\
				wattrset(status_win, COLOR_PAIR(pairs + 1 + pair));\
				pair = (pair + 1) % 7;\
				mvwaddnstr(status_win, 0, x, str, cols - x);\
				x -= 2;\
			} while (FALSE)
	//wrefresh_ADDSTR("P: %u", (unsigned int )getpid());
	wrefresh_ADDSTR("S: %u/%u", current_state, states - 1);
	wrefresh_ADDSTR("R: 0x%08x", (unsigned int )hash(executable_arc4_s, 0x100));
	wrefresh_ADDSTR("E: %u/%u", (unsigned int )(timestamp - record.timestamp), (unsigned int )timestamp);
	const unsigned int durr = (unsigned int )dur + 1;
	if (durr < frame_rate) {
		wrefresh_ADDSTR("D: 1/%u", frame_rate / durr);
	}
	else {
		wrefresh_ADDSTR("D: %u", durr / frame_rate);
	}
	wrefresh_ADDSTR("T: 0/%u", (unsigned int )(*executable_turns + surplus_turns));
	wrefresh_ADDSTR("F: %u/%u", record.count - previous_count, record.count);
	wrefresh_ADDSTR("I: %s", name);
	//mvaddnstr("DrCo", name);
	free(str);

	return NO_PROBLEM;
}

/**
Draws the save state menu in the middle of a window.

@param win The window.
**/
problem_t draw_menu(void) {
	const chtype attr = COLOR_PAIR(pairs + 2);
	const chtype ch = attr | ' ';
	const chtype lrs = attr | '|';
	const chtype tbs = attr | '-';
	const chtype tblr = attr | '+';

	/*
	Draws the menu window.
	*/
	wbkgd(menu_win, ch);
	wborder(menu_win, lrs, lrs, tbs, tbs, tblr, tblr, tblr, tblr);

	/*
	Draws the list subwindow.
	*/
	wbkgd(menu_states_win, ch);
	wborder(menu_states_win, lrs, lrs, tbs, tbs, tblr, tblr, tblr, tblr);
	int y, x;
	getmaxyx(menu_states_win, y, x);

	int row = 1;
	int left_col = 1;
	int right_col = x;
	{
		const size_t size = strlen(interface_left_more) + 1;
		mvwaddstr(menu_states_win, row, left_col, interface_left_more);
		left_col += size;
	}
	{
		const size_t size = strlen(interface_right_more) + 1;
		mvwaddstr(menu_states_win, row, right_col - size, interface_right_more);
		right_col -= size;
	}
	const int middle_col = (right_col + left_col) / 2;
	int dec_col = middle_col;
	int inc_col = middle_col;
	bool left_more = FALSE;
	bool right_more = FALSE;
	int diff = -1;
	int state = current_state;
	while (TRUE) {
		const char * interface_left;
		const char * interface_right;
		if (state > 0 && state < states && shm.pids[state] == 0) {
			interface_left = interface_left_unused;
			interface_right = interface_right_unused;
		}
		else {
			interface_left = interface_left_used;
			interface_right = interface_right_used;
		}
		const size_t len = strlen(interface_left)
				+ uintlen(state)
				+ strlen(interface_right) + 2;
		int col;
		if (inc_col == dec_col) {//TODO use div
			inc_col += 1 + (len - 1) / 2;
			dec_col -= len / 2;
			col = dec_col;
		}
		else if (diff < 0) {
			dec_col -= len;
			col = inc_col;
		}
		else {
			inc_col += len;
			col = dec_col - len;//heuristic
		}
		if (dec_col < left_col) {
			left_more = TRUE;
		}
		if (inc_col > right_col) {
			right_more = TRUE;
		}
		if ((diff < 0 && !left_more) || (diff > 0 && !right_more)) {
			const size_t size = len + 1;
			char * const buf = malloc(size);
			if (buf == NULL) {
				error(MALLOC_PROBLEM);
			}
			else {
				snprintf(buf, size, "%s%u%s",
						interface_left,
						state,
						interface_right);
				mvwaddstr(menu_states_win, row, col, buf);
				free(buf);
			}
		}
		state += diff;
		diff = - (diff + SGN(diff));
		if (state < 0) {
			left_more = TRUE;
		}
		if (state > states) {
			right_more = TRUE;
		}
		//fprintf(stderr, "(%d)", diff);
		if (left_more && right_more) break;
	}
	if (left_more) {
		mvwaddstr(menu_states_win, row, 1, interface_left_more);
	}
	if (right_more) {
		const size_t size = strlen(interface_right_more) + 1;
		mvwaddstr(menu_states_win, row, x - size, interface_right_more);
	}

	/*
	Draws the preview subwindow.
	*/
	for (unsigned int row = 0, halfrow = 0; row < rows - 1; row += 2, halfrow++) {
		for (unsigned int col = 0, halfcol = 0; col < cols; col += 2, halfcol++) {
			mvwaddch(menu_chs_win, halfrow, halfcol, shm.chs[current_state][row][col]);
		}
	}

	return NO_PROBLEM;
}

problem_t draw_interface(WINDOW * const win) {
	init_interface();
	draw_status();
	um_wrefresh(status_win);
	if (inactive) {
		draw_menu();
		um_wrefresh(menu_win);
		um_wrefresh(menu_chs_win);
		um_wrefresh(menu_states_win);
	}
	uninit_interface();//TODO move

	return NO_PROBLEM;
}

#endif
