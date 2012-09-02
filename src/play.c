/**
Plays records.

TODO rewrite

@file play.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#include <stddef.h>//NULL

#include <curses.h>//*w*, WINDOW

#include "util.h"//intern, KEY_EOF, TIMER_RATE
#include "cfg.h"//*_key
#include "rec.h"//record
#include "exec.h"//
#include "arc4.h"//
#include "lib.h"//

#include "play.h"

int next_key(WINDOW * const win) {
	if (record.current == NULL) {
		wtimeout(win, 0);
		return KEY_EOF;
	}
	int key;
	if (record.current->duration == 0) {//seed frame
		cfg_timestamp += record.current->value;
		arc4_inject((unsigned long int )cfg_timestamp, exec_arc4_calls_automatic_load);
		(*exec_saves)++;
		key = KEY_NULL;
	}
	else {//key frame
		const int delay = record.current->duration * NAP_RESOLUTION / frame_rate;
		wtimeout(win, delay);
		const int some_key = wgetch(win);
		if (some_key == cfg_play_key) {
			wtimeout(win, 0);
			int some_other_key;
			do {
				some_other_key = wgetch(win);
			} while (some_other_key != cfg_play_key);
		}
		else if (some_key == cfg_stop_key) {
			record.current = NULL;
		}
		key = record.current->value;
	}
	record.current = record.current->next;
	return key;
}
