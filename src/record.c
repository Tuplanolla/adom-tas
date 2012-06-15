/**
Provides recording utilities.
**/
#ifndef RECORD_C
#define RECORD_C

#include <stdlib.h>
#include <time.h>

#include "record.h"

/**
Creates a new record.

@return The new record.
**/
void init_record(record_t * record) {
	record->first = NULL;
	record->last = NULL;
	record->count = 0;
	record->time = 0;
}

/**
Removes all frames from a record.

@param record The target record.
**/
void clear_record(record_t * record) {
	frame_t * current = record->first;
	record->first = NULL;
	record->last = NULL;
	record->count = 0;
	record->time = 0;
	while (current != NULL) {
		frame_t * previous = current;
		current = current->next;
		free(previous);
	}
}

/**
Adds a frame to a record.

@param record The target record.
@param duration The input or the duration of the frame.
@param value The key or the time difference of the frame.
@return The new frame.
**/
frame_t * add_frame(record_t * record, const int duration, const int value) {
	frame_t * current = malloc(sizeof (frame_t));
	current->duration = duration;
	current->value = value;
	current->next = NULL;
	if (record->count == 0) {//the first
		record->first = current;
	}
	else {//the rest
		record->last->next = current;
	}
	record->last = current;
	record->count++;
	return current;
}

/**
Adds a <code>KEY_INPUT</code> frame to a record.

@param record The target record.
@param duration The duration of the frame.
@param key The key of the frame.
@return The new frame.
**/
frame_t * add_key_frame(record_t * record, const unsigned char duration, const int key) {
	return add_frame(record, (int )duration, key);
}

/**
Adds a <code>TIME_INPUT</code> and <code>SEED_INPUT</code> frame to a record.

@param record The target record.
@param time The time of the frame.
@return The new frame.
**/
frame_t * add_seed_frame(record_t * record, const time_t time) {
	const time_t step_time = time-record->time;
	record->time = time;
	return add_frame(record, 0, (int )step_time);
}

#endif
