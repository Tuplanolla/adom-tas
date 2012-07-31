/**
Manages inputs and outputs.
**/
#ifndef PUT_C
#define PUT_C

#include <stddef.h>//size_t, NULL
#include <stdio.h>//*open, *close, *read, *write, FILE

#include "util.h"//FALSE, TRUE
#include "prob.h"//*_PROBLEM
#include "log.h"//error, warning, note
#include "rec.h"//record*

#include "put.h"

/**
Saves a record.

@param path The record location.
@return The error code.
**/
problem_d fwritep(const char * const path) {
	FILE * const stream = fopen(path, "wb");
	if (stream == NULL) {
		return error(OUTPUT_OPEN_PROBLEM);
	}
	unsigned char header[1024];
	memset(header, 0, sizeof header);
	if (fwrite(header, sizeof header, 1, stream)) {//TODO header
		error(OUTPUT_WRITE_PROBLEM);
	}
	frame_d * frame = record.first;
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

@param path The record location.
@return The error code.
**/
problem_d freadp(const char * const path) {
	FILE * const stream = fopen(path, "rb");
	if (stream == NULL) {
		return error(INPUT_OPEN_PROBLEM);
	}
	if (fseek(stream, 1024, SEEK_SET) == -1) {//TODO fix
		error(INPUT_READ_PROBLEM);
	}
	while (TRUE) {
		unsigned char duration;
		int value;
		size_t result = fread(&duration, sizeof duration, 1, stream);
		result += fread(&value, sizeof value, 1, stream);
		if (result != 2) {
			break;
		}
		if (add_frame(duration, value) == NULL) {
			error(MALLOC_PROBLEM);
			break;
		}
	}
	if (feof(stream) == 0) {
		error(INPUT_READ_PROBLEM);
	}
	if (fclose(stream) == EOF) {
		return error(INPUT_CLOSE_PROBLEM);
	}
	return NO_PROBLEM;
}

#endif
