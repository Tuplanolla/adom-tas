/**
Provides general-purpose utilities.
**/
#ifndef UTIL_H
#define UTIL_H

#include <string.h>
#include <math.h>

/**
Lists the boolean values.
**/
#undef TRUE
#undef FALSE
#undef bool
enum bool_e {
	FALSE,
	TRUE
};
typedef enum bool_e bool;

/**
Sets the print format of a byte.

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
#define SWAP(x, y) {\
		unsigned char SWAP_z[sizeof (x) == sizeof (y) ? sizeof (x) : -1];\
		memcpy(SWAP_z, &y, sizeof (x));\
		memcpy(&y, &x, sizeof (x));\
		memcpy(&x, SWAP_z, sizeof (x));\
	}

/**
Returns the string length of an integer.

@param x The integer.
@return The string length.
**/
inline size_t intlen(int x) {
	size_t len = 1;
	if (x < 0) {
		len++;
		x = -x;
	}
	while (x > 9) {
		len++;
		x /= 10;
	}
	return len;
}

/**
Returns the hash code of a byte array.

@param array The byte array.
@param size The length of the byte array.
@return The hash code.
**/
inline int hash(const unsigned char * array, const size_t size) {
	const int prime = 31;
	int result = 1;
	for (size_t index = 0; index < size; index++) {
		result = prime*result+(int )array[index];
		index++;
	}
	return result;
}

/**
Converts a string to a standard stream.

An additional <code>else</code> can be appended. Not a very good idea.

@param str The string to convert.
@return The standard stream.
**/
#undef STDSTR
#define STDSTR(stream, str) \
	if (strcmp(str, "null") == 0) stream = NULL;\
	else if (strcmp(str, "stdin") == 0) stream = stdin;\
	else if (strcmp(str, "stdout") == 0) stream = stdout;\
	else if (strcmp(str, "stderr") == 0) stream = stderr;

#endif
