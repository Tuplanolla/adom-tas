/**
Draws the graphical user interface.

TODO refactor

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef GUI_C
#define GUI_C

#include <stdlib.h>

#include <curses.h>

#include "util.h"
#include "exec.h"
#include "shm.h"
#include "prob.h"//problem_d, *_PROBLEM
#include "rec.h"
#include "def.h"
#include "lib.h"
#include "log.h"
#include "cfg.h"

#include "gui.h"

/**
The amount of colors.
**/
const size_t colors = sizeof interface_colors / sizeof *interface_colors;

chtype attr;
chtype eattr;
chtype ch;
chtype lrs;
chtype tbs;
chtype tblr;

/**
The status bar window.
**/
WINDOW * status_win = NULL;
/**
The information window.
**/
WINDOW * info_win = NULL;
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
/**
The map overlay window.
**/
WINDOW * overlay_win = NULL;

/**
Uninitializes the interface.
**/
problem_d uninit_gui(void) {
	if (delwin(status_win)) {
		error(DELWIN_PROBLEM);
	}
	if (delwin(info_win)) {
		error(DELWIN_PROBLEM);
	}
	if (delwin(menu_win)) {
		error(DELWIN_PROBLEM);
	}
	if (delwin(menu_chs_win)) {
		error(DELWIN_PROBLEM);
	}
	if (delwin(menu_states_win)) {
		error(DELWIN_PROBLEM);
	}
	if (delwin(overlay_win)) {
		error(DELWIN_PROBLEM);
	}

	return NO_PROBLEM;
}

/**
Initializes the interface.
**/
problem_d init_gui(void) {
	/*
	Initializes the custom color pairs.
	*/
	for (size_t color = 0; color < colors; color++) {
		if (monochrome) {
			if (um_init_pair(pairs + color, COLOR_BLACK, interface_colors[color]) == ERR) {
				return error(INIT_PAIR_PROBLEM);
			}
		}
		else {
			if (um_init_pair(pairs + color, COLOR_WHITE, COLOR_BLACK) == ERR) {
				return error(INIT_PAIR_PROBLEM);
			}
		}
	}
	if (um_init_pair(pairs + colors, COLOR_BLACK, COLOR_WHITE) == ERR) {
		return error(INIT_PAIR_PROBLEM);
	}
	if (um_init_pair(pairs + colors + 1, COLOR_WHITE, COLOR_WHITE) == ERR) {
		return error(INIT_PAIR_PROBLEM);
	}

	/*
	Creates the windows.
	*/
	status_win = newwin(1, cols, rows - 1, 0);
	if (status_win == NULL) {
		return error(NEWWIN_PROBLEM);
	}
	info_win = newwin(rows, cols, 0, 0);
	if (info_win == NULL) {
		return error(NEWWIN_PROBLEM);
	}
	const int nlines = rows / 2;
	const int ncols = cols / 2;
	const int begin_y = rows / 4 - 1;
	const int begin_x = cols / 4;
	menu_win = newwin(1 + nlines + 1, 1 + ncols + 1, begin_y - 1, begin_x - 1);
	if (menu_win == NULL) {
		return error(NEWWIN_PROBLEM);
	}
	menu_states_win = newwin(3, 1 + ncols + 1, begin_y + nlines, begin_x - 1);
	if (menu_states_win == NULL) {
		return error(NEWWIN_PROBLEM);
	}
	menu_chs_win = newwin(nlines, ncols, begin_y, begin_x);
	if (menu_chs_win == NULL) {
		return error(NEWWIN_PROBLEM);
	}
	overlay_win = newwin(rows - 5, cols, 2, 0);
	if (overlay_win == NULL) {
		return error(NEWWIN_PROBLEM);
	}

	/*
	Initializes the line drawing characters.
	*/
	attr = COLOR_PAIR(pairs + colors);
	eattr = COLOR_PAIR(pairs + colors + 1);
	ch = attr | ' ';
	lrs = eattr | '|';
	tbs = eattr | '-';
	tblr = eattr | '+';

	return NO_PROBLEM;
}

/**
Draws the status bar in the bottom of a window from right to left.

@return The error code.
**/
problem_d draw_status(WINDOW * const win) {
	wclear(status_win);
	const size_t size = cols + 1;
	char * const str = malloc(size);
	if (str == NULL) {
		return error(MALLOC_PROBLEM);
	}
	int left_pos = 0;
	short int pair = 0;
	#define draw_status_ADDSTR(format, __GVA_ARGS__...) do {\
			snprintf(str, size, format, ##__GVA_ARGS__);\
			wattrset(status_win, COLOR_PAIR(pairs + pair));\
			pair = (pair + 1) % colors;\
			mvwaddstr(status_win, 0, left_pos, str);\
			left_pos += strlen(str) + 1;\
			if (monochrome) {\
				left_pos++;\
			}\
		} while (0)
	char buf[9];
	winnstr(win, buf, 8);
	draw_status_ADDSTR(buf);
	//draw_status_ADDSTR("P: %u", (unsigned int )getpid());
	const size_t inputs = sizeof previous_inputs / sizeof *previous_inputs;
	char input_string[4 * inputs];
	input_string[0] = '\0';
	for (size_t input = 0; input < inputs; input++) {
		if (previous_inputs[input] != 0) {
			strcat(input_string, key_code(previous_inputs[input]));
		}
	}
	draw_status_ADDSTR("I: %s", input_string);
	draw_status_ADDSTR("F: %u/%u", record.frames - previous_frames, record.frames);
	draw_status_ADDSTR("T: 0/%u", turns);
	if (current_duration < frame_rate) {
		draw_status_ADDSTR("D: 1/%u", frame_rate / current_duration);
	}
	else {
		draw_status_ADDSTR("D: %u", current_duration / frame_rate);
	}
	draw_status_ADDSTR("E: %ld/%ld", (long int )(timestamp - record.timestamp), (long int )timestamp);
	draw_status_ADDSTR("R: 0x%08x", (unsigned int )hash(exec_arc4_s, 0x100));
	draw_status_ADDSTR("S: %d/%d", current_state, states - 1);
	free(str);

	wrefresh(status_win);

	return NO_PROBLEM;
}

/**
Draws the save state menu in the middle of a window.

@return The error code.
**/
problem_d draw_menu(void) {
	/*
	Draws the menu window.
	*/
	wclear(menu_win);
	wbkgd(menu_win, ch);
	wborder(menu_win, lrs, lrs, tbs, tbs, tblr, tblr, tblr, tblr);

	int y, x;

	/*
	Draws the list subwindow.
	*/
	wclear(menu_states_win);
	wbkgd(menu_states_win, ch);
	wborder(menu_states_win, lrs, lrs, tbs, tbs, tblr, tblr, tblr, tblr);

	getmaxyx(menu_states_win, y, x);
	int row = 1;
	int left_edge = 0;
	int right_edge = x - 1;
	left_edge += strlen(interface_left_more) + 1;
	right_edge -= strlen(interface_right_more) + 1;
	int left_pos;
	int right_pos;
	bool left_end = FALSE;
	bool right_end = FALSE;
	bool left_more = FALSE;
	bool right_more = FALSE;
	int diff = 0;
	int state = current_state;
	do {
		const char * interface_left = "";
		const char * interface_right = "";
		if (state > 0 && state < states) {
			if (shm.pids[state] == 0) {
				interface_left = interface_left_unused;
				interface_right = interface_right_unused;
			}
			else {
				interface_left = interface_left_used;
				interface_right = interface_right_used;
			}
		}
		const size_t len = strlen(interface_left)
				+ intlen(state)
				+ strlen(interface_right);
		int col = -1;
		if (diff == 0) {//move to the center
			diff++;//determines the direction
			left_pos = left_edge + (right_edge - left_edge - len - 1) / 2 + 1;
			col = left_pos;
			right_pos = left_pos + len - 1;
		}
		else if (diff > 0) {//move left
			if (!left_more) {
				left_pos -= len;
				left_pos--;//spacing
				col = left_pos;
				if (left_pos <= left_edge) {
					if (state > 0) {
						left_more = TRUE;
					}
					left_end = TRUE;
					col = -1;
				}
			}
			if (state <= 0) {
				left_end = TRUE;
				col = -1;
			}
		}
		else {//move right
			if (!right_more) {
				right_pos++;//spacing
				col = right_pos + 1;
				right_pos += len;
				if (right_pos >= right_edge) {
					if (state < states) {
						right_more = TRUE;
					}
					right_end = TRUE;
					col = -1;
				}
			}
			if (state >= states) {
				right_end = TRUE;
				col = -1;
			}
		}
		if (col != -1) {
			const size_t size = len + 1;
			char * const buf = malloc(size);
			if (buf == NULL) {
				error(MALLOC_PROBLEM);
			}
			else {
				snprintf(buf, size, "%s%u%s",
						interface_left, state, interface_right);
				mvwaddstr(menu_states_win, row, col, buf);
				free(buf);
			}
		}
		state += diff;
		diff = - (diff + SGN(diff));
	} while (!(left_end && right_end));
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
	wclear(menu_chs_win);
	getmaxyx(menu_chs_win, y, x);
	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			mvwaddch(menu_chs_win, row / 2, col / 2, shm.chs[current_state][row][col]);
		}
	}

	/*
	Refreshes the menu window.
	*/
	wrefresh(menu_win);
	wrefresh(menu_chs_win);
	wrefresh(menu_states_win);

	return NO_PROBLEM;
}

/**
Draws the save state menu in the middle of a window.

@return The error code.
**/
problem_d draw_info(void) {
	/*
	Draws the info window.
	*/
	wclear(info_win);
	wbkgd(info_win, ch);
	wborder(info_win, lrs, lrs, tbs, tbs, tblr, tblr, tblr, tblr);

	int y, x;
	getmaxyx(info_win, y, x);
	mvwaddstr(info_win, y / 2, x / 2, "Yes!");

	/*
	Refreshes the info window.
	*/
	wrefresh(info_win);

	return NO_PROBLEM;
}

/**
Draws the overlay.

@param win The window.
@return The error code.
**/
problem_d draw_overlay(WINDOW * const win) {
	for (int row = 0; row < rows - 5; row++) {
		for (int col = 0; col < cols; col++) {
			const chtype ch = mvwinch(win, row + 2, col);
			const chtype sch = A_CHARTEXT & ch;
			if (sch == ' ') {
				const attr_t attr = COLOR_PAIR(8) | A_BOLD;
				wattron(overlay_win, attr);
				const unsigned char terrain = (*exec_terrain)[row * cols + col];
				mvwaddch(overlay_win, row, col, exec_terrain_chars[terrain]);
				const unsigned char object = (*exec_objects)[row * cols + col];
				if (object != 0) {
					mvwaddch(overlay_win, row, col, exec_object_chars[object]);
				}
				wattroff(overlay_win, attr);
			}
			else {
				mvwaddch(overlay_win, row, col, ch);
			}
		}
	}
	for (int row = 0; row < rows - 5; row++) {
		if (*exec_items == NULL) break;
		for (int col = 0; col < cols; col++) {
			const exec_map_item_d * item = (*exec_items)[row * cols + col];
			if (item != NULL) {
				while (item->next != NULL) {
					item = item->next;
					if (item->item != NULL) {
						const exec_item_data_d i = exec_item_data[item->item->type];
						int color = i.color;
						if (color == -1) {
							color = exec_material_colors[i.material];
						}
						attr_t attr = COLOR_PAIR(color);
						if (color >= 8) {
							attr |= A_BOLD;
						}
						wattron(overlay_win, attr);
						chtype ch = ' ';
						if (item->item->type == 53 || item->item->type == 580) {
							ch = exec_item_chars[22];
						}
						else {
							ch = exec_item_chars[i.category];
						}
						mvwaddch(overlay_win, row, col, ch);
						wattroff(overlay_win, attr);
					}
				}
			}
		}
	}
	const exec_map_monster_d * monster = *exec_monsters;
	if (monster != NULL) {
		while (monster->next != NULL) {
			monster = monster->next;
			if (monster->monster != NULL) {
				const exec_monster_data_d m = exec_monster_data[monster->monster->type];
				attr_t attr = COLOR_PAIR(m.color);
				if (m.color >= 8) {
					attr |= A_BOLD;
				}
				wattron(overlay_win, attr);
				mvwaddch(overlay_win, monster->monster->y, monster->monster->x, (chtype )m.character);
				wattroff(overlay_win, attr);
			}
		}
	}
	wrefresh(overlay_win);

	return NO_PROBLEM;
}

problem_d draw_gui(WINDOW * const win) {
	if (!hidden) {
		if (in_game) {
			draw_overlay(win);
		}
		PROPAGATE(draw_status(win));
		if (inactive) {
			if (menuinfo) {
				PROPAGATE(draw_menu());
			}
			else {
				PROPAGATE(draw_info());
			}
		}
	}

	return NO_PROBLEM;
}

#endif
