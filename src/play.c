/**
Plays records.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef PLAY_C
#define PLAY_C

#include <stddef.h>//NULL

void * nowhere;

int nothing(void) {
	if (current_frame != NULL) {//TODO move this
		if (current_frame->duration == 0) {
			timestamp += current_frame->value;
			iarc4((unsigned int )timestamp, exec_arc4_calls_automatic_load);
			current_frame = current_frame->next;
			return 0;
		}
		else {
			struct timespec req;
			bool out_of_variable_names = FALSE;
			if (current_frame->duration >= frame_rate) out_of_variable_names = TRUE;
			req.tv_sec = (time_t )(out_of_variable_names ? current_frame->duration : 0);
			req.tv_nsec = out_of_variable_names ? 0l : 1000000000l / frame_rate * current_frame->duration;
			nanosleep(&req, NULL);//TODO use a better timer
			const int yield = current_frame->value;
			current_frame = current_frame->next;
			return yield;
		}
	}
	return -1;
}

#endif
