/**
Provides general-purpose utilities.
**/
#ifndef UTIL_H
#define UTIL_H

/**
Lists the boolean values.
**/
#undef TRUE
#undef FALSE
#undef bool
enum bool_e {
	FALSE,
	TRUE,
};
typedef enum bool_e bool;

/**
Swaps two variables.

@param x The first variable.
@param y The second variable.
@return An argument list of the bits.
**/
#define SWAP(x, y) {\
		unsigned char SWAP_z[sizeof (x) == sizeof (y) ? sizeof (x) : -1];\
		memcpy(SWAP_z, &y, sizeof (x));\
		memcpy(&y, &x, sizeof (x));\
		memcpy(&x, SWAP_z, sizeof (x));\
	}

/**
Returns the bits of a byte.

@param byte The byte.
@return An argument list of the bits.
**/
#define BITS(byte) \
	byte&0b10000000 ? 1 : 0,\
	byte&0b01000000 ? 1 : 0,\
	byte&0b00100000 ? 1 : 0,\
	byte&0b00010000 ? 1 : 0,\
	byte&0b00001000 ? 1 : 0,\
	byte&0b00000100 ? 1 : 0,\
	byte&0b00000010 ? 1 : 0,\
	byte&0b00000001 ? 1 : 0

/**
Sets the print format of a byte.

Works in conjuction with the <code>BITS</code> macro:
<pre>
printf("0x%x = 0b"BYTE, 42, BITS(42));
</pre>
**/
#define BYTE "%d%d%d%d%d%d%d%d"

/**
Returns the page boundary of a pointer.

@param pointer The pointer.
@return A pointer to the page boundary.
**/
#define PAGE(pointer) ((void *)(((int )pointer)-((int )pointer)%getpagesize()))

/**
Returns the page size of an object.

@param size The size of the object.
@return The page size of the object.
**/
#define PAGE_SIZE(size) ((size_t )((1+(sizeof (no_yes_no)-1)/getpagesize())*getpagesize()))

/*
Bad things live in header files.
*/
struct frame_s {//optimized for minimum size and abstracted away later
	//bool seed;//the input is actually a reseed and processing ends here
	//if the input is a reseed then duration is 0
	int duration;//the duration of the frame (in quarter seconds) if an actual input
	int input;//the input as an ncurses bitstring if an actual input
	//if duration is 0 then the time is the input
	//int time;//the current time if a reseed
	struct frame_s *next;//the next frame (forming a linked list)
};
typedef struct frame_s frame_t;

frame_t *get_first_frame();
frame_t *get_last_frame();
void frame_add(bool reseed, unsigned int duration, int input, unsigned int time);
void frame_remove(frame_t *frame_pointer);

#endif
