/**
Manages saving and loading records.

@file put.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#include <stddef.h>//size_t, NULL
#include <stdio.h>//*open, *close, *read, *write, FILE
#include <string.h>//str*

#include "util.h"//*PACK, FALSE, TRUE
#include "prob.h"//probno, *_PROBLEM
#include "log.h"//log_*
#include "rec.h"//*_frame, record

#include "put.h"

/**
Saves a record.

@param path The record location.
@return The number of objects written.
**/
size_t put_fwrite(const char * const path) {
	/*
	Opens the file.
	*/
	FILE * const stream = fopen(path, "wb");
	if (stream == NULL) {
		probno = log_error(OUTPUT_OPEN_PROBLEM);
		return 0;
	}

	/*
	Writes the header.
	*/
	unsigned char header[1024] = {0};

	unsigned char * position = &header[0];
	strcpy((char * )position, &record_type[0]);
	position += (ptrdiff_t )sizeof record_type;
	strcpy((char * )position, &record.author[0]);
	position += (ptrdiff_t )sizeof record.author;
	strcpy((char * )position, &record.executable[0]);
	position += (ptrdiff_t )sizeof record.executable;
	strcpy((char * )position, &record.comments[0]);
	position += (ptrdiff_t )sizeof record.comments;
	PACK(position, record.category);
	PACK(position, record.frames);
	PACK(position, record.time);
	PACK(position, record.turns);

	size_t result = 0;
	if (fwrite(&header[0], sizeof header, 1, stream) != 1) {
		probno = log_error(OUTPUT_WRITE_PROBLEM);
		goto hell;
	}
	result++;

	/*
	Writes the chunks.
	*/
	const frame_d * frame = record.first;
	unsigned char chunk[sizeof frame->duration + sizeof frame->value];
	while (frame != NULL) {
		position = &chunk[0];
		PACK(chunk, frame->duration);
		position += (ptrdiff_t )sizeof frame->duration;
		PACK(chunk, frame->value);
		position += (ptrdiff_t )sizeof frame->value;
		if (fwrite(&chunk[0], sizeof chunk, 1, stream) != 1) {
			probno = log_error(OUTPUT_WRITE_PROBLEM);
			goto hell;
		}
		else {
			result++;
		}
		frame = frame->next;
	}

	/*
	Closes the file.
	*/
	hell: if (fclose(stream) == EOF) {
		probno = log_error(OUTPUT_CLOSE_PROBLEM);
	}

	return result;
}

/**
Loads a record.

@param path The record location.
@return The number of objects read.
**/
size_t put_fread(const char * const path) {
	/*
	Opens the file.
	*/
	FILE * const stream = fopen(path, "rb");
	if (stream == NULL) {
		probno = log_error(INPUT_OPEN_PROBLEM);
		return 0;
	}

	/*
	Reads the header.
	*/
	unsigned char header[1024];
	size_t result = 0;
	if (fread(&header[0], sizeof header, 1, stream) != 1) {
		probno = log_error(INPUT_READ_PROBLEM);
	}

	unsigned char * position = &header[0];
	if (strcmp((char * )position, &record_type[0]) != 0) {
		probno = log_error(INPUT_FORMAT_PROBLEM);
		goto hell;
	}
	position += (ptrdiff_t )sizeof record_type;
	strcpy(record.author, (char * )position);
	position += (ptrdiff_t )sizeof record.author;
	strcpy(record.executable, (char * )position);
	position += (ptrdiff_t )sizeof record.executable;
	strcpy(record.comments, (char * )position);
	position += (ptrdiff_t )sizeof record.comments;
	UNPACK(record.category, position);
	position += (ptrdiff_t )sizeof record.category;
	UNPACK(record.frames, position);
	position += (ptrdiff_t )sizeof record.frames;
	UNPACK(record.time, position);
	position += (ptrdiff_t )sizeof record.time;
	UNPACK(record.turns, position);
	position += (ptrdiff_t )sizeof record.turns;
	result++;

	/*
	Reads the chunks.
	*/
	frame_d frame;
	unsigned char chunk[sizeof frame.duration + sizeof frame.value];
	do {
		if (fread(&chunk[0], sizeof chunk, 1, stream) != 1) {
			if (feof(stream) == 0) {
				probno = log_error(INPUT_READ_PROBLEM);
				goto hell;
			}
			else {
				break;
			}
		}
		else if (rec_add_frame(frame.duration, frame.value) == NULL) {
			probno = log_error(INPUT_FRAME_PROBLEM);
			goto hell;
		}
		else {
			result++;
		}
	} while (TRUE);

	/*
	Closes the file.
	*/
	hell: if (fclose(stream) == EOF) {
		probno = log_error(INPUT_CLOSE_PROBLEM);
	}

	return result;
}
