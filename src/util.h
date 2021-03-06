/**
@file util.h
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>//size_t
#include <stdio.h>//FILE
#include <string.h>//mem*
#include <stdint.h>//uintptr_t
#include <inttypes.h>//PRIxPTR
#include <limits.h>//CHAR_BIT
#include <unistd.h>//*page*

#include <curses.h>//KEY_*

#include "gnu.h"//__*__

#undef FALSE
#undef TRUE
#undef bool
/**
Boolean values.
**/
enum bool_e {
	FALSE,
	TRUE
};
typedef enum bool_e bool;

/**
Marks the beginning of a multi-statement macro.
**/
#define BEGIN do {
/**
Marks the end of a multi-statement macro.
**/
#define END } while (FALSE)

/**
Touches a variable.

Used to suppress warnings relating to unused parameters.
**/
#define UNUSED(x) ((void )(x))

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
#define SUBNULL ((void * )-1)

/**
The print format of a fixed-width byte.

Works in conjuction with the <code>BITS</code> macro:
<pre>
printf("0x%x = 0b" BITSF, 42, BITS(42));
</pre>
**/
#define BITSF "%d%d%d%d%d%d%d%d"
/**
The print parameters of a fixed-width byte.

@param byte The byte.
@return The argument list for the bits.
**/
#define BITS(byte) \
	(byte) & 1 << 7 ? 1 : 0,\
	(byte) & 1 << 6 ? 1 : 0,\
	(byte) & 1 << 5 ? 1 : 0,\
	(byte) & 1 << 4 ? 1 : 0,\
	(byte) & 1 << 3 ? 1 : 0,\
	(byte) & 1 << 2 ? 1 : 0,\
	(byte) & 1 << 1 ? 1 : 0,\
	(byte) & 1 << 0 ? 1 : 0

/**
The print format of a fixed-width pointer.

Works in conjuction with the <code>PTRS</code> macro:
<pre>
volatile int x;
printf("&x = " PTRF, PTRS(&x));
</pre>
**/
#define PTRF "0x%0*" PRIxPTR
/**
The print parameters of a fixed-width pointer.

@param ptr The pointer.
@return The argument list for the pointer.
**/
#define PTRS(ptr) \
		(int )((sizeof (void *) * CHAR_BIT - 1) / 4 + 1),\
		(uintptr_t )(ptr)

/**
Returns the smaller of two numbers.

@param x The first number.
@param y The second number.
@return The smaller number.
**/
#define MIN(x, y) ((x) < (y) ? (x) : (y))

/**
Returns the bigger of two numbers.

@param x The first number.
@param y The second number.
@return The bigger number.
**/
#define MAX(x, y) ((x) > (y) ? (x) : (y))

/**
Returns the sign of a number.

@param x The number.
@return The sign.
**/
#define SGN(x) ((x) > 0 ? 1 : (x) < 0 ? -1 : 0)

/**
Increments a number modularly.

Values outside the range are bound to the opposite extreme:
<pre>
x  INC(x, 3, 7)
1  7
2  7
3  4
4  5
5  6
6  7
7  3
8  3
</pre>

@param x The number.
@param l The lower limit.
@param u The upper limit.
@return The incremented number.
**/
#define INC(x, l, u) ((x) = (x) < (l) ? (u) : (x) < (u) ? (x) + 1 : (l))
/**
Decrements a number modularly.

Values outside the range are bound to the opposite extreme:
<pre>
x  DEC(x, 3, 7)
9  3
8  3
7  6
6  5
5  4
4  3
3  7
2  7
</pre>

@param x The number.
@param l The lower limit.
@param u The upper limit.
@return The decremented number.
**/
#define DEC(x, l, u) ((x) = (x) > (u) ? (l) : (x) > (l) ? (x) - 1 : (u))

/**
Returns the page boundary of a pointer.

@param pointer The pointer.
@return A pointer to the page boundary.
**/
#define PAGE(pointer) ((void * )(((intptr_t )(pointer)) - ((intptr_t )(pointer)) % getpagesize()))
/**
Returns the page size of an object.

@param object The object.
@return The page size of the object.
**/
#define PAGE_SIZE(object) ((size_t )((((int )sizeof (object) - 1) / getpagesize() + 1) * getpagesize()))

/**
Swaps two variables.

@param x The first variable.
@param y The second variable.
@return An argument list of the bits.
**/
#define SWAP(x, y) BEGIN\
		unsigned char SWAP_z[sizeof (x) == sizeof (y) ? sizeof (x) : 0];\
		memcpy(SWAP_z, &(y), sizeof (x));\
		memcpy(&(y), &(x), sizeof (x));\
		memcpy(&(x), SWAP_z, sizeof (x));\
	END

/**
Packs a variable for storage.

@param dest The packed array of bytes.
@param src The variable to pack.
**/
#define PACK(dest, src) BEGIN\
		size_t PACK_byte = 0;\
		while (PACK_byte < sizeof src) {\
			dest[PACK_byte] = (unsigned char )(src >> (PACK_byte * CHAR_BIT));\
			PACK_byte++;\
		}\
	END
/**
Unpacks a variable for use.

@param dest The unpacked variable.
@param src The array of bytes to unpack.
**/
#define UNPACK(dest, src) BEGIN\
		dest = 0;\
		size_t UNPACK_byte = sizeof src;\
		while (UNPACK_byte > 0) {\
			UNPACK_byte--;\
			dest |= (unsigned char )src[UNPACK_byte] << (UNPACK_byte * CHAR_BIT);\
		}\
	END

/**
The missing permission modifier for the System V's SHM library.
**/
#define SHM_RW 0

/**
The Shift modifier for the New Cursor Optimization library.
**/
#define MOD_SHIFT 12
/**
The Ctrl modifier for the New Cursor Optimization library.
**/
#define MOD_CTRL 24
/**
The Meta modifier for the New Cursor Optimization library.
**/
#define MOD_META 27
/**
The null key for the New Cursor Optimization library.
**/
#define KEY_NULL (KEY_MIN - 2)
/**
The end-of-file key for the New Cursor Optimization library.
**/
#define KEY_EOF (KEY_MAX + 1)
/**
The timer resolution of the New Cursor Optimization library.
**/
#define NAP_RESOLUTION 1000

size_t intlen(int x);
size_t uintlen(unsigned int x);
int hash(const unsigned char * array, size_t size);
char * astresc(const char * const str) __attribute__ ((malloc));
char * astrrep(const char * haystack, const char * needle, const char * replacement) __attribute__ ((malloc));
int copy(const char * dest, const char * src);
int smart_copy(const char * dest, const char * src);
FILE * stdstr(const char * str);

#endif
