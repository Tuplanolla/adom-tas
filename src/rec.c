/**
Manages records.

Records are built around linked lists.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef REC_C
#define REC_C

#include <stdlib.h>//*alloc, free, NULL
#include <time.h>//time_t

#include "util.h"//intern

#include "rec.h"

/**
The record.
**/
intern record_d record = {
	.first = NULL,
	.current = NULL,
	.last = NULL,
	.count = 0,
	.timestamp = 0
};

/**
The frame rate.

Choosing <code>sqrt(1 << CHAR_BIT)</code> as the frame rate creates a balanced time distribution.
For a byte the minimum frame time is 0.0625 seconds and the maximum 16 seconds.
**/
intern unsigned short int frame_rate = 16;

/**
Removes all frames from a record.
**/
void clear_record(void) {
	frame_d * frame = record.first;
	record.first = NULL;
	record.current = NULL;
	record.last = NULL;
	record.count = 0;
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
frame_d * add_frame(const unsigned short int duration, const long int value) {
	frame_d * frame = malloc(sizeof *frame);
	if (frame == NULL) {
		return NULL;
	}
	frame->duration = duration;
	frame->value = value;
	frame->next = NULL;
	if (record.count == 0) {
		record.first = frame;
	}
	else {
		record.last->next = frame;
	}
	record.last = frame;
	record.count++;
	return frame;
}

/**
Adds a <code>KEY_INPUT</code> frame to a record.

@param duration The duration of the frame.
@param key The key of the frame.
@return The new frame.
**/
frame_d * add_key_frame(const unsigned short int duration, const int key) {
	return add_frame(duration, key);
}

/**
Adds a <code>TIME_INPUT</code> and <code>SEED_INPUT</code> frame to a record.

@param timestamp The time of the frame.
@return The new frame.
**/
frame_d * add_seed_frame(const time_t timestamp) {
	const time_t difference = timestamp - record.timestamp;
	record.timestamp = timestamp;
	return add_frame(0, (long int )difference);
}

#endif
