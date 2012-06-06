/**
Provides general-purpose utilities.
**/
#ifndef UTIL_C
#define UTIL_C

#include <stdlib.h>

#include "util.h"

/*
This is an old implementation that most likely sucks, but works for now.
*/
frame_t *first_frame = NULL, *last_frame = NULL;
void set_first_frame(frame_t *ff) {
	first_frame = ff;
}
void set_last_frame(frame_t *ff) {
	last_frame = ff;
}
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
	return NULL;//Hahaha, no!
}

#endif
