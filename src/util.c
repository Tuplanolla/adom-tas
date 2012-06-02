/**
Provides general-purpose utilities.
**/
#ifndef UTIL_C
#define UTIL_C

#include <curses.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "util.h"

/*
This is an old implementation that most likely sucks, but works for now.
*/
frame_t *first_frame = NULL, *last_frame = NULL;
frame_t *get_first_frame() {
	return first_frame;
}
frame_t *get_last_frame() {
	return last_frame;
}
unsigned int frame_count = 0;
//add an object (split into add_reseed and add_input later)
void frame_add(bool reseed, unsigned int duration, int input, unsigned int time) {
	frame_t *new_frame;
	new_frame = (frame_t *)malloc(sizeof (frame_t));

	new_frame->duration = reseed ? 0 : (int )duration;
	new_frame->input = (int )input;

	//first object
	if (first_frame == NULL && last_frame == NULL) {
		first_frame = new_frame;
		last_frame = new_frame;
		new_frame->next = NULL;
		frame_count++;
		return;
	}

	//other object
	last_frame->next = new_frame;
	last_frame = new_frame;
	new_frame->next = NULL;
	frame_count++;
	return;

}

//get an object
frame_t *frame_get(void) {
	//Hahaha, no!
}

//remove an object
void frame_remove(frame_t *frame_pointer) {
	//not an object
	if (frame_pointer == NULL) return;//null error

	//first and last object
	if (first_frame == last_frame) {
		first_frame = NULL;
		last_frame = NULL;
		free(first_frame);
		frame_count--;
		return;
	}

	//first object
	if (frame_pointer == first_frame) {
		first_frame = first_frame->next;
		free(frame_pointer);
		frame_count--;
		return;
	}

	//previous object
	frame_t *frame_temporary = first_frame;
	while (frame_temporary->next != frame_pointer) {
		frame_temporary = frame_temporary->next;
		if (frame_temporary == NULL) return;//unexpected end error
	}

	//last object
	if (frame_pointer == last_frame) {
		frame_temporary->next = NULL;
		last_frame = frame_temporary;
		free(frame_pointer);
		frame_count--;
		return;
	}

	//other object
	frame_temporary->next = frame_pointer->next;
	free(frame_pointer);
	frame_count--;
	return;
}

#endif
