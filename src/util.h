/**
Provides general-purpose macros.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>//size_t
#include <stdio.h>//FILE
#include <string.h>//mem*
#include <unistd.h>//getpagesize

/**
The boolean values.
**/
#undef FALSE
#undef TRUE
enum bool_e {
	FALSE,
	TRUE
};
#undef bool
typedef enum bool_e bool;

/**
General-purpose functions.
**/
size_t intlen(int x);
int hash(const unsigned char * array, const size_t size);
char * astrrep(const char * haystack, const char * needle, const char * replacement);
FILE * stdstr(const char * str);

/**
An alternative <code>NULL</code> for error checking.

Works with special functions:
<pre>
shm = shmat(shmid, NULL, 0);
if (shm == SUBNULL) {
	return errno;
}
</pre>
**/
#undef SUBNULL
#define SUBNULL ((void * )-1)

/**
The print format of a byte.

Works in conjuction with the <code>BITS</code> macro:
<pre>
printf("0x%x = 0b"BYTE, 42, BITS(42));
</pre>
**/
#undef BYTE
#define BYTE "%d%d%d%d%d%d%d%d"

/**
Returns the bits of a byte.

@param byte The byte.
@return An argument list of the bits.
**/
#undef BITS
#define BITS(byte) \
	byte & 0b10000000 ? 1 : 0,\
	byte & 0b01000000 ? 1 : 0,\
	byte & 0b00100000 ? 1 : 0,\
	byte & 0b00010000 ? 1 : 0,\
	byte & 0b00001000 ? 1 : 0,\
	byte & 0b00000100 ? 1 : 0,\
	byte & 0b00000010 ? 1 : 0,\
	byte & 0b00000001 ? 1 : 0

/*
Returns the smaller of two numbers.

@param x The first number.
@param y The second number.
@return The smaller number.
*/
#undef MIN
#define MIN(x, y) (x < y ? x : y)

/*
Returns the bigger of two numbers.

@param x The first number.
@param y The second number.
@return The bigger number.
*/
#undef MAX
#define MAX(x, y) (x > y ? x : y)

/**
Returns the page boundary of a pointer.

@param pointer The pointer.
@return A pointer to the page boundary.
**/
#undef PAGE
#define PAGE(pointer) ((void * )(((int )(pointer))-((int )(pointer))%getpagesize()))

/**
Returns the page size of an object.

@param object The object.
@return The page size of the object.
**/
#undef PAGE_SIZE
#define PAGE_SIZE(object) ((size_t )((1+(sizeof (object)-1)/getpagesize())*getpagesize()))

/**
Swaps two variables.

@param x The first variable.
@param y The second variable.
@return An argument list of the bits.
**/
#undef SWAP
#define SWAP(x, y) do {\
		unsigned char SWAP_z[sizeof (x) == sizeof (y) ? sizeof (x) : -1];\
		memcpy(SWAP_z, &y, sizeof (x));\
		memcpy(&y, &x, sizeof (x));\
		memcpy(&x, SWAP_z, sizeof (x));\
	} while (FALSE)

#endif
