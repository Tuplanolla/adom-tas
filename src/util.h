/**
Provides general-purpose utilities.
**/
#ifndef UTIL_H
#define UTIL_H

/**
Lists the boolean values.
Some other library already does this.
**/
/*
enum bool_e {
	FALSE,
	TRUE,
};
typedef enum bool_e bool;
*/

/**
Swaps two variables.

@param x The first variable.
@param y The second variable.
@return An argument list of the bits.
**/
#define SWAP(x, y) {\
		unsigned char SWAP_cache[sizeof (x) == sizeof (y) ? (signed )sizeof (x) : -1];\
		memcpy(SWAP_cache, &y, sizeof (x));\
		memcpy(&y, &x, sizeof (x));\
		memcpy(&x, SWAP_cache, sizeof (x));\
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

#endif
