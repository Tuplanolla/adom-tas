/**
Draws the graphical user interface.

TODO refactor

@file gui.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#include <stdlib.h>

#include <curses.h>

#include "util.h"
#include "prob.h"//problem_d, *_PROBLEM
#include "log.h"//log_*
#include "def.h"//def_*
#include "cfg.h"//cfg_*
#include "exec.h"//exec_*
#include "shm.h"//shm
#include "rec.h"//record
#include "arc4.h"
#include "key.h"
#include "lib.h"

#include "gui.h"

/**
The amount of colors.
**/
static const size_t colors = sizeof def_gui_colors / sizeof *def_gui_colors;

static chtype attr;
static chtype eattr;
static chtype ch;
static chtype lrs;
static chtype tbs;
static chtype tblr;

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
int gui_uninit(void) {
	if (delwin(status_win)) {
		log_error(DELWIN_PROBLEM);
	}
	if (delwin(info_win)) {
		log_error(DELWIN_PROBLEM);
	}
	if (delwin(menu_win)) {
		log_error(DELWIN_PROBLEM);
	}
	if (delwin(menu_chs_win)) {
		log_error(DELWIN_PROBLEM);
	}
	if (delwin(menu_states_win)) {
		log_error(DELWIN_PROBLEM);
	}
	if (delwin(overlay_win)) {
		log_error(DELWIN_PROBLEM);
	}

	return 0;
}

/**
Initializes the interface.
**/
int gui_init(void) {
	/*
	Initializes the custom color pairs.
	*/
	for (size_t color = 0; color < colors; color++) {
		if (cfg_monochrome) {
			if (orig_init_pair(pairs + color, COLOR_WHITE, COLOR_BLACK) == ERR) {
				return log_error(INIT_PAIR_PROBLEM);
			}
		}
		else {
			if (orig_init_pair(pairs + color, COLOR_BLACK, def_gui_colors[color]) == ERR) {
				return log_error(INIT_PAIR_PROBLEM);
			}
		}
	}
	if (orig_init_pair(pairs + colors, COLOR_BLACK, COLOR_WHITE) == ERR) {
		return log_error(INIT_PAIR_PROBLEM);
	}
	if (orig_init_pair(pairs + colors + 1, COLOR_WHITE, COLOR_WHITE) == ERR) {
		return log_error(INIT_PAIR_PROBLEM);
	}

	/*
	Creates the windows.
	*/
	status_win = newwin(1, cfg_cols, cfg_rows - 1, 0);
	if (status_win == NULL) {
		return log_error(NEWWIN_PROBLEM);
	}
	info_win = newwin(cfg_rows, cfg_cols, 0, 0);
	if (info_win == NULL) {
		return log_error(NEWWIN_PROBLEM);
	}
	const int nlines = cfg_rows / 2;
	const int ncols = cfg_cols / 2;
	const int begin_y = cfg_rows / 4 - 1;
	const int begin_x = cfg_cols / 4;
	menu_win = newwin(1 + nlines + 1, 1 + ncols + 1, begin_y - 1, begin_x - 1);
	if (menu_win == NULL) {
		return log_error(NEWWIN_PROBLEM);
	}
	menu_states_win = newwin(3, 1 + ncols + 1, begin_y + nlines, begin_x - 1);
	if (menu_states_win == NULL) {
		return log_error(NEWWIN_PROBLEM);
	}
	menu_chs_win = newwin(nlines, ncols, begin_y, begin_x);
	if (menu_chs_win == NULL) {
		return log_error(NEWWIN_PROBLEM);
	}
	overlay_win = newwin(cfg_rows - 5, cfg_cols, 2, 0);
	if (overlay_win == NULL) {
		return log_error(NEWWIN_PROBLEM);
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

	return 0;
}

/**
Draws the status bar in the bottom of a window from right to left.

@return The error code.
**/
static int gui_draw_status(WINDOW * const win) {
	wclear(status_win);
	const size_t size = cfg_cols + 1;
	char * const str = malloc(size);
	if (str == NULL) {
		return log_error(MALLOC_PROBLEM);
	}
	int left_pos = 0;
	short int pair = 0;
	#define draw_status_ADDSTR(format, __GVA_ARGS__...) BEGIN\
			snprintf(str, size, format, ##__GVA_ARGS__);\
			wattrset(status_win, COLOR_PAIR(pairs + pair));\
			pair = (pair + 1) % colors;\
			mvwaddstr(status_win, 0, left_pos, str);\
			left_pos += strlen(str) + 1;\
			if (cfg_monochrome) {\
				left_pos++;\
			}\
		END
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
	draw_status_ADDSTR("F: %llu/%llu", record.frames - previous_record_frames, record.frames);
	draw_status_ADDSTR("T: 0/%ld", turns);
	if (current_duration < frame_rate) {
		draw_status_ADDSTR("D: 1/%u", frame_rate / current_duration);
	}
	else {
		draw_status_ADDSTR("D: %u", current_duration / frame_rate);
	}
	draw_status_ADDSTR("E: %ld/%ld", (long int )(cfg_timestamp - record.timestamp), (long int )cfg_timestamp);
	draw_status_ADDSTR("R: 0x%08x", (unsigned int )hash(exec_arc4_s, 0x100));
	draw_status_ADDSTR("S: %d/%d", current_save, cfg_saves - 1);
	free(str);

	wrefresh(status_win);

	return 0;
}

/**
Draws the save state menu in the middle of a window.

@return The error code.
**/
static int gui_draw_menu(void) {
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
	left_edge += strlen(def_gui_left_more) + 1;
	right_edge -= strlen(def_gui_right_more) + 1;
	int left_pos;
	int right_pos;
	bool left_end = FALSE;
	bool right_end = FALSE;
	bool left_more = FALSE;
	bool right_more = FALSE;
	int diff = 0;
	int state = current_save;
	do {
		const char * interface_left = "";
		const char * interface_right = "";
		if (state > 0 && state < cfg_saves) {
			if (shared.pids[state] == 0) {
				interface_left = def_gui_left_unused;
				interface_right = def_gui_right_unused;
			}
			else {
				interface_left = def_gui_left_used;
				interface_right = def_gui_right_used;
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
					if (state < cfg_saves) {
						right_more = TRUE;
					}
					right_end = TRUE;
					col = -1;
				}
			}
			if (state >= cfg_saves) {
				right_end = TRUE;
				col = -1;
			}
		}
		if (col != -1) {
			const size_t size = len + 1;
			char * const buf = malloc(size);
			if (buf == NULL) {
				log_error(MALLOC_PROBLEM);
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
		mvwaddstr(menu_states_win, row, 1, def_gui_left_more);
	}
	if (right_more) {
		const size_t size = strlen(def_gui_right_more) + 1;
		mvwaddstr(menu_states_win, row, x - size, def_gui_right_more);
	}

	/*
	Draws the preview subwindow.
	*/
	wclear(menu_chs_win);
	getmaxyx(menu_chs_win, y, x);
	for (int row = 0; row < cfg_rows; row++) {
		for (int col = 0; col < cfg_cols; col++) {
			mvwaddch(menu_chs_win, row / 2, col / 2, shared.chs[current_save][row][col]);
		}
	}

	/*
	Refreshes the menu window.
	*/
	wrefresh(menu_win);
	wrefresh(menu_chs_win);
	wrefresh(menu_states_win);

	return 0;
}

/**
Draws the save state menu in the middle of a window.

@return The error code.
**/
static int gui_draw_info(void) {
	/*
	Draws the info window.
	*/
	wclear(info_win);
	wbkgd(info_win, ch);
	wborder(info_win, lrs, lrs, tbs, tbs, tblr, tblr, tblr, tblr);

	int y, x;
	getmaxyx(info_win, y, x);
	mvwaddstr(info_win, y / 2, x / 2, "Yes!");//TODO info

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
/*static*/ int gui_draw_overlay(WINDOW * const win) {
	for (int row = 0; row < cfg_rows - 5; row++) {
		for (int col = 0; col < cfg_cols; col++) {
			const chtype ch = mvwinch(win, row + 2, col);
			const chtype sch = A_CHARTEXT & ch;
			if (sch == ' ') {
				const attr_t attr = COLOR_PAIR(8) | A_BOLD;
				wattron(overlay_win, attr);
				const unsigned char terrain = (*exec_terrain)[row * cfg_cols + col];
				mvwaddch(overlay_win, row, col, exec_terrain_chars[terrain]);
				const unsigned char object = (*exec_objects)[row * cfg_cols + col];
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
	for (int row = 0; row < cfg_rows - 5; row++) {
		if (*exec_items == NULL) break;
		for (int col = 0; col < cfg_cols; col++) {
			const exec_map_item_d * item = (*exec_items)[row * cfg_cols + col];
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

	return 0;
}

int gui_draw(WINDOW * const win) {
	if (!options.gui_hidden) {
		if (options.gui_overlay_active) {
			gui_draw_overlay(win);
		}
		gui_draw_status(win);
	}
	if (options.gui_menu_active) {
		gui_draw_menu();
	}
	else if (options.gui_info_active) {
		gui_draw_info();
	}

	return 0;
}
