/**
Manages inputs and outputs.
**/
#ifndef PUT_C
#define PUT_C

#include <stddef.h>//size_t, NULL
#include <stdio.h>//*read, *write, FILE

#include "util.h"//FALSE, TRUE
#include "problem.h"//..., *_PROBLEM?
#include "record.h"//*_frame, record_t

#include "put.h"

/**
Saves a record.

@param stream The destination stream.
@param record The record.
@return The amount of frames written.
**/
size_t fwritep(FILE * const stream, record_t * const record) {//TODO refactor
	size_t result = 0;
	frame_t * frame = record->first;
	while (frame != NULL) {
		size_t subresult;
		subresult += fwrite(&frame->duration, sizeof frame->duration, 1, stream);
		subresult += fwrite(&frame->value, sizeof frame->value, 1, stream);
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
size_t freadp(FILE * const stream, record_t * const record) {
	size_t result = 0;
	while (TRUE) {
		size_t subresult = 0;
		unsigned char duration;
		int value;
		subresult += fread(&duration, sizeof duration, (size_t )1, stream);
		subresult += fread(&value, sizeof value, (size_t )1, stream);
		if (subresult == 0) break;
		add_frame(record, duration, value);
		result++;
	}
	return result;
}

#endif
