/**
Manages records.

Records are built around linked lists.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#include <stdlib.h>//*alloc, free, NULL
#include <string.h>//mem*
#include <time.h>//time_t

#include "prob.h"//probno, *_PROBLEM
#include "log.h"//log_*

#include "rec.h"

/**
The type identifier used in record file headers.
**/
const char record_type[4] = "TAS";

/**
The record.
**/
record_d record = {
	.first = NULL,
	.current = NULL,
	.last = NULL,
	.author = {0},
	.executable = {0},
	.comments = {0},
	.category = 0,
	.frames = 0,
	.time = 0,
	.turns = 0,
	.timestamp = 0
};

/**
The frame rate.

Choosing <code>sqrt(1 << CHAR_BIT)</code> as the frame rate creates a balanced time distribution.
For a byte the minimum frame time is 0.0625 seconds and the maximum 16 seconds.
**/
unsigned short int frame_rate = 16;

/**
Removes all frames from a record.
**/
void rec_clear(void) {
	frame_d * frame = record.first;
	record.first = NULL;
	record.current = NULL;
	record.last = NULL;
	memset(record.author, 0, sizeof record.author);
	memset(record.executable, 0, sizeof record.executable);
	memset(record.comments, 0, sizeof record.comments);
	record.category = 0;
	record.frames = 0;
	record.time = 0;
	record.turns = 0;
	record.timestamp = 0;
	while (frame != NULL) {
		frame_d * const previous = frame;
		frame = frame->next;
		free(previous);
	}
}

/**
Adds a frame to a record.

@param duration The input or the duration of the frame.
@param value The key or the time difference of the frame.
@return The new frame if successful and <code>NULL</code> otherwise.
**/
frame_d * rec_add_frame(const unsigned short int duration, const long int value) {
	frame_d * frame = malloc(sizeof *frame);
	if (frame == NULL) {
		probno = log_error(MALLOC_PROBLEM);
		return NULL;
	}
	frame->duration = duration;
	frame->value = value;
	frame->next = NULL;
	if (record.frames == 0) {
		record.first = frame;
	}
	else {
		record.last->next = frame;
	}
	record.last = frame;
	record.frames++;
	return frame;
}

/**
Adds a <code>KEY_INPUT</code> frame to a record.

@param duration The duration of the frame.
@param key The key of the frame.
@return The new frame.
**/
frame_d * rec_add_key_frame(const unsigned short int duration, const int key) {
	return rec_add_frame(duration, key);
}

/**
Adds a <code>TIME_INPUT</code> and <code>SEED_INPUT</code> frame to a record.

@param timestamp The time of the frame.
@return The new frame.
**/
frame_d * rec_add_seed_frame(const time_t timestamp) {
	const time_t difference = timestamp - record.timestamp;
	record.timestamp = timestamp;
	return rec_add_frame(0, (long int )difference);
}
