/**
Provides recording utilities.
**/
#ifndef RECORD_H
#define RECORD_H

#include <time.h>//time_t

extern unsigned char frame_rate;

/**
Represents a recorded frame.

Three kinds of inputs exist:
<pre>
enum input_e {
	NO_INPUT,
	KEY_INPUT,
	TIME_INPUT,
	SEED_INPUT
};
typedef enum input_e input_t;
</pre>
<code>KEY_INPUT</code> represents pressing a key,
<code>TIME_INPUT</code> represents changing the system time and
<code>SEED_INPUT</code> represents reseeding the random number generator.
The variables required to represent a frame depend on the inputs:
<pre>
struct frame_s {
	input_t input;
	int key;
	time_t time;
	unsigned char duration;
	struct frame_s * next;
};
typedef struct frame_s frame_t;
</pre>
Since a <code>time_t</code> can be treated as an <code>int</code> or a <code>long</code>,
only <code>KEY_INPUT</code> inputs are visible and
<code>TIME_INPUT</code> inputs are only used with the next <code>SEED_INPUT</code> inputs
the struct can be condensed:
<pre>
struct frame_s {
	unsigned char duration;//duration == 0 ? input = KEY_INPUT : input = SEED_INPUT
	int value;//duration != 0 ? key = value : time += value
	struct frame_s * next;
};
typedef struct frame_s frame_t;
</pre>
Thus only five bytes are needed.

@var duration The type of the input or the duration of the frame.
@var value The key or the time difference of the frame.
@var next The next frame.
**/
struct frame_s {
	unsigned char duration;
	int value;
	struct frame_s * next;
};
typedef struct frame_s frame_t;

/**
Represents a recorded collection of frames.

@var first The first frame.
@var last The last frame.
@var count The amount of frames.
@var time The system time of the previous <code>SEED_INPUT</code> frame.
**/
struct record_s {
	frame_t * first;
	frame_t * last;
	unsigned int count;
	time_t time;
};
typedef struct record_s record_t;

void init_record(record_t * record);
void clear_record(record_t * record);
frame_t * add_frame(record_t * record, unsigned char duration, int value);
frame_t * add_key_frame(record_t * record, unsigned char duration, int key);
frame_t * add_seed_frame(record_t * record, time_t time);

#endif
