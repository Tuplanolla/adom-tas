/**
Manages inputs and outputs.
**/
#ifndef PUT_C
#define PUT_C

#include <stdio.h>

#include "util.h"
#include "problem.h"
#include "log.h"
#include "record.h"
#include "put.h"

/**
Saves a record.

@param stream The destination stream.
@param record The record.
@return The amount of frames written.
**/
size_t fwritep(FILE * stream, record_t * record) {
	size_t result = 0;
	frame_t * frame = record->first;
	while (frame != NULL) {
		size_t subresult;
		subresult += fwrite(&frame->duration, sizeof (frame->duration), 1, stream);
		subresult += fwrite(&frame->value, sizeof (frame->value), 1, stream);
		result++;
		frame = frame->next;
	}
	return result;
}

/**
Loads a record.

@param stream The source stream.
@param record The record.
@return The amount of frames read.
**/
size_t freadp(FILE * stream, record_t * record) {
	size_t result = 0;
	while (TRUE) {
		int subresult = 0;
		int duration;
		int value;
		subresult += fread(&duration, sizeof (duration), 1, stream);
		subresult += fread(&value, sizeof (value), 1, stream);
		if (subresult == 0) break;
		add_frame(record, duration, value);
		result++;
	}
	return result;
}

#endif
