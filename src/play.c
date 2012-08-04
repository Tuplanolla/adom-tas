/**
Plays records.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef PLAY_C
#define PLAY_C

#include <stddef.h>//NULL

#include <curses.h>//*w*, WINDOW

#include "util.h"//intern, KEY_EOF, TIMER_RATE
#include "cfg.h"//*_key
#include "rec.h"//record

#include "play.h"

int next_key(WINDOW * const win) {
	if (record.current == NULL) {
		wtimeout(win, 0);
		return KEY_EOF;
	}
	int key;
	if (record.current->duration == 0) {//seed frame
		timestamp += record.current->value;
		iarc4((unsigned long int )timestamp, exec_arc4_calls_automatic_load);
		key = KEY_NAK;
	}
	else {//key frame
		const int delay = record.current->duration * TIMER_RATE / frame_rate;
		wtimeout(win, delay);
		const int some_key = um_wgetch(win);
		if (some_key == play_key) {
			wtimeout(win, 0);
			int some_other_key;
			do {
				some_other_key = um_wgetch(win);
			} while (some_other_key != play_key);
		}
		else if (some_key == stop_key) {
			record.current = NULL;
		}
		key = record.current->value;
	}
	record.current = record.current->next;
	return key;
}

#endif
