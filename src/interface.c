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
#include "problem.h"
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

WINDOW * status_win = NULL;
WINDOW * menu_win = NULL;
WINDOW * menu_chwin = NULL;
WINDOW * menu_statewin = NULL;

problem_t init_interface(void) {
	/*
	Initializes the custom color pairs.
	*/
	for (size_t color = 0; color < 7; color++) {
		init_pair(pairs + color, colors[color] == COLOR_BLACK ? COLOR_WHITE : COLOR_BLACK, colors[color]);
	}

	/*
	Creates the windows.
	*/
	status_win = newwin(1, cols, rows - 1, 0);
	const int nlines = rows / 2;
	const int ncols = cols / 2;
	const int begin_y = rows / 4 - 1;
	const int begin_x = cols / 4;
	menu_win = newwin(nlines + 4, ncols + 2, begin_y - 1, begin_x - 1);
	menu_chwin = newwin(nlines, ncols, begin_y, begin_x);
	menu_statewin = newwin(3, ncols, begin_y + rows / 2, begin_x);

	return NO_PROBLEM;
}

problem_t uninit_interface(void) {
	delwin(status_win);
	delwin(menu_win);
	delwin(menu_chwin);
	delwin(menu_statewin);

	return NO_PROBLEM;
}

/**
Draws the status bar in the bottom of a window from right to left.

@param win The window.
**/
void draw_status() {
	int x = cols;
	short pair = 0;
	char * const str = malloc(cols);
	#define wrefresh_ADDSTR(format, ...) do {\
				snprintf(str, cols, format, __VA_ARGS__);\
				x -= strlen(str);\
				wattrset(status_win, COLOR_PAIR(pairs + pair));\
				pair = (pair + 1) % 7;\
				mvwaddnstr(status_win, 0, x, str, cols - x);\
				x -= 2;\
			} while (FALSE)
	//wrefresh_ADDSTR("P: %u", (unsigned int )getpid());
	wrefresh_ADDSTR("S: %u/%u", current_state, states - 1);
	wrefresh_ADDSTR("R: 0x%08x", (unsigned int )hash(executable_arc4_s, 0x100));
	wrefresh_ADDSTR("E: %u/%u", (unsigned int )(timestamp - record.timestamp), (unsigned int )timestamp);
	const unsigned int durr = (unsigned int )dur + 1;
	if (durr < frame_rate) wrefresh_ADDSTR("D: 1/%u", frame_rate / durr);
	else wrefresh_ADDSTR("D: %u", durr / frame_rate);
	wrefresh_ADDSTR("T: 0/%u", (unsigned int )(*executable_turns + surplus_turns));
	wrefresh_ADDSTR("F: %u/%u", record.count - previous_count, record.count);
	wrefresh_ADDSTR("I: %s", name);
	//mvaddnstr("DrCo", name);
	free(str);
}

/**
Draws the save state menu in the middle of a window.

@param win The window.
**/
void draw_menu() {
	chtype ch = (chtype )' ' | COLOR_PAIR(pairs);
	wborder(menu_win, ch, ch, ch, ch, ch, ch, ch, ch);
	wbkgd(menu_win, COLOR_PAIR(pairs));
	wbkgd(menu_statewin, COLOR_PAIR(pairs));
	unsigned int ws_x = 1;
	size_t len = strlen("<");
	mvwaddnstr(menu_statewin, 1, ws_x, "<", len);
	ws_x += len + 1;
	unsigned int state = 1;
	while (ws_x < cols / 2 - 2 && state < states) {
		len = 1 + uintlen(state) + 1;
		char * buf = malloc(len + 1);
		const bool occupied = shm.pids[state] != 0;
		snprintf(buf, len + 1, "%c%u%c", occupied ? '[' : ' ', state, occupied ? ']' : ' ');
		mvwaddnstr(menu_statewin, 1, ws_x, buf, len);
		free(buf);
		const char * indicator = "^";
		if (state == current_state) {//TODO intlen / 2 as well
			mvwaddnstr(menu_statewin, 2, ws_x + 1 - strlen(indicator) / 2, indicator, strlen(indicator));
		}
		state++;
		ws_x += len + 1;
	}
	len = strlen(">");
	mvwaddnstr(menu_statewin, 1, cols / 2 + 1 - len, ">", len);
	wattrset(menu_win, A_NORMAL);
	for (unsigned int row = 0, halfrow = 0; row < rows - 1; row += 2, halfrow++) {
		for (unsigned int col = 0, halfcol = 0; col < cols; col += 2, halfcol++) {
			mvwaddch(menu_chwin, halfrow, halfcol, shm.chs[current_state][row][col]);
		}
	}
}

void draw_interface(WINDOW * const win) {
	init_interface();
	draw_status();
	um_wrefresh(status_win);
	if (inactive) {
		draw_menu();
		um_wrefresh(menu_win);
		um_wrefresh(menu_chwin);
		um_wrefresh(menu_statewin);
	}
	uninit_interface();//TODO move
}

#endif
