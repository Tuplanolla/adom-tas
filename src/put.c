/**
Manages inputs and outputs.
**/
#ifndef PUT_C
#define PUT_C

#include <stddef.h>//size_t, NULL
#include <stdio.h>//*open, *close, *read, *write, FILE

#include "util.h"//FALSE, TRUE
#include "problem.h"//*_PROBLEM
#include "log.h"//error, warning, note
#include "record.h"//record*

#include "put.h"

intern record_t record;

/**
Saves a record.

@param stream The destination stream.
@param record The record.
@return The error code.
**/
problem_t fwritep(const char * const path) {//TODO refactor
	FILE * const stream = fopen(path, "wb");
	if (stream == NULL) {
		return error(OUTPUT_OPEN_PROBLEM);
	}
	frame_t * frame = record.first;
	while (frame != NULL) {
		size_t result;
		result = fwrite(&frame->duration, sizeof frame->duration, 1, stream);
		result += fwrite(&frame->value, sizeof frame->value, 1, stream);
		if (result != 2) {
			error(OUTPUT_WRITE_PROBLEM);
			break;
		}
		frame = frame->next;
	}
	if (fclose(stream) == EOF) {
		return error(OUTPUT_CLOSE_PROBLEM);
	}
	return NO_PROBLEM;
}

/**
Loads a record.

@param stream The source stream.
@param record The record.
@return The error code.
**/
problem_t freadp(const char * const path) {
	FILE * const stream = fopen(path, "rb");
	if (stream == NULL) {
		return error(INPUT_OPEN_PROBLEM);
	}
	while (TRUE) {
		size_t result;
		unsigned char duration;
		int value;
		result = fread(&duration, sizeof duration, 1, stream);
		result += fread(&value, sizeof value, 1, stream);
		if (result != 2) {
			error(INPUT_READ_PROBLEM);
			break;
		}
		add_frame(duration, value);
	}
	if (fclose(stream) == EOF) {
		return error(INPUT_CLOSE_PROBLEM);
	}
	return NO_PROBLEM;
}

#endif
