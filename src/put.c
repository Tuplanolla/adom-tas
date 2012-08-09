/**
Manages saving and loading records.
**/
#ifndef PUT_C
#define PUT_C

#include <stddef.h>//size_t, NULL
#include <stdio.h>//*open, *close, *read, *write, FILE
#include <limits.h>//CHAR_BIT

#include "util.h"//FALSE, TRUE
#include "prob.h"//*_PROBLEM
#include "log.h"//error, warning, note
#include "rec.h"//*_frame, record

#include "put.h"

/**
The header record files start with.
**/
unsigned char header[1024];

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

	memset(header, 0, sizeof header);

	char * position = header;
#define fwritep_MEMCPY(src) do {\
		for (size_t byte = 0; byte < sizeof src; byte++) {\
			memcpy(position + byte, (unsigned char )((size_t )src >> (byte * CHAR_BIT)), 1);\
		}\
		position += (ptrdiff_t )sizeof src;\
	} while (0)
	fwritep_MEMCPY(record_type);
	fwritep_MEMCPY(record.author);
	fwritep_MEMCPY(record.executable);
	fwritep_MEMCPY(record.comments);
	fwritep_MEMCPY(record.category);
	fwritep_MEMCPY(record.frames);
	fwritep_MEMCPY(record.time);
	fwritep_MEMCPY(record.turns);

	if (fwrite(header, sizeof header, 1, stream) != 1) {
		error(OUTPUT_WRITE_PROBLEM);
	}

	const frame_d * frame = record.first;
	while (frame != NULL) {
		size_t result = fwrite(&frame->duration, sizeof frame->duration, 1, stream);
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

	char * position = 0;
#define freadp_MEMCPY(dest) do {\
		for (size_t byte = 0; byte < sizeof dest; byte++) {\
			memcpy((void * )(unsigned char )((size_t )dest >> (byte * CHAR_BIT)), position + byte, 1);\
		}\
		position += (ptrdiff_t )sizeof dest;\
	} while (0)
	char type[sizeof record_type];
	freadp_MEMCPY(type);
	if (memcmp(type, record_type, sizeof type) != 0) {
		error(INPUT_FORMAT_PROBLEM);
	}
	freadp_MEMCPY(record.author);
	freadp_MEMCPY(record.executable);
	freadp_MEMCPY(record.comments);
	freadp_MEMCPY(record.category);
	freadp_MEMCPY(record.frames);
	freadp_MEMCPY(record.time);
	freadp_MEMCPY(record.turns);

	if (fseek(stream, sizeof header, SEEK_SET) == -1) {
		error(INPUT_READ_PROBLEM);
	}

	do {
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
	} while (TRUE);
	if (feof(stream) == 0) {
		error(INPUT_READ_PROBLEM);
	}

	if (fclose(stream) == EOF) {
		return error(INPUT_CLOSE_PROBLEM);
	}

	return NO_PROBLEM;
}

#endif
