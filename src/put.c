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
Outputs the frames (for recording).
**/
size_t fwritep(record_t * record, const char * output_file) {
	FILE * output_stream = fopen(output_file, "wb");
	if (output_stream == NULL) {
		return error(OUTPUT_ACCESS_PROBLEM);
	}
	size_t result = 0;
	frame_t * current_frame = record->first;
	while (current_frame != NULL) {
		result += fwrite(&current_frame->duration, sizeof (current_frame->duration), 1, output_stream);
		result += fwrite(&current_frame->value, sizeof (current_frame->value), 1, output_stream);
		current_frame = current_frame->next;
	}
	fclose(output_stream);
	return result;
}

/**
Inputs the frames (for playback).
**/
size_t freadp(record_t * record, const char * input_file) {
	FILE * input_stream = fopen(input_file, "rb");
	if (input_stream == NULL) {
		return error(INPUT_ACCESS_PROBLEM);
	}
	size_t result = 0;
	while (TRUE) {
		int subresult = 0;
		int duration;
		int value;
		subresult += fread(&duration, sizeof (duration), 1, input_stream);
		subresult += fread(&value, sizeof (value), 1, input_stream);
		if (subresult == 0) break;
		add_frame(record, duration, value);
	}
	if (feof(input_stream)) /*error*/;
	clearerr(input_stream);
	fclose(input_stream);
	return result;
}

#endif
