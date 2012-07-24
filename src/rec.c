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
intern record_t record = {
	.first = NULL,
	.last = NULL,
	.count = 0,
	.timestamp = 0
};

/**
The frame rate.

Choosing <code>sqrt(1 << 8 * sizeof duration)</code> as the frame rate creates a balanced time distribution.
For a byte the minimum frame time is 0.0625 seconds and the maximum 16 seconds.
**/
intern unsigned char frame_rate = 16;

/**
Removes all frames from a record.
**/
void clear_record(void) {
	frame_t * frame = record.first;
	record.first = NULL;
	record.last = NULL;
	record.count = 0;
	record.timestamp = 0;
	while (frame != NULL) {
		frame_t * previous = frame;
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
frame_t * add_frame(const unsigned char duration, const int value) {
	frame_t * frame = malloc(sizeof (frame_t));
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
frame_t * add_key_frame(const unsigned char duration, const int key) {
	return add_frame(duration, key);
}

/**
Adds a <code>TIME_INPUT</code> and <code>SEED_INPUT</code> frame to a record.

@param timestamp The time of the frame.
@return The new frame.
**/
frame_t * add_seed_frame(const time_t timestamp) {
	const time_t difference = timestamp - record.timestamp;
	record.timestamp = timestamp;
	return add_frame(0, (int )difference);
}

#endif
