/**
Provides general-purpose functions.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef UTIL_C
#define UTIL_C

#include <stdlib.h>//*alloc, size_t, NULL
#include <stdio.h>//*print*, std*, FILE
#include <string.h>//str*

#include "util.h"

/**
Returns the string length of an integer.

@param x The integer.
@return The string length.
**/
size_t intlen(int x) {
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
int hash(const unsigned char * array, const size_t size) {
	const int prime = 31;
	int result = 1;
	for (size_t index = 0; index < size; index++) {
		result = prime * result + array[index];
		index++;
	}
	return result;
}

/**
Replaces the first occurrence of a string in a string with another string.

Allocates enough memory for the return value.

All occurrences can be replaced with a loop, assuming no recursion occurs:
<pre>
strcpy(haystack, "an ample example");
strcpy(needle, "ample");
strcpy(replacement, "act");
while (strstr(haystack, needle)) {
	char * result = astrrep(haystack, needle, replacement);
	SWAP(haystack, result);
	free(result);
}
printf("%s\n", haystack);
</pre>

@param haystack The string to search from.
@param needle The string to search for.
@param replacement The string to replace with.
@return The resulting string.
**/
char * astrrep(const char * const haystack, const char * const needle, const char * const replacement) {
	if (haystack == NULL) {
		return NULL;
	}
	const size_t haystack_size = strlen(haystack) + 1;
	const char * needle_position = NULL;
	if (needle != NULL && replacement != NULL) {
		const char * needle_position = strstr(haystack, needle);
	}
	if (needle_position == NULL) {
		char * result = malloc(haystack_size);
		strcpy(result, haystack);
		return result;
	}
	const size_t needle_length = strlen(needle);
	const size_t replacement_length = strlen(replacement);
	const size_t result_size = haystack_size - needle_length + replacement_length;
	char * result = malloc(result_size);
	const char * haystack_position = haystack;
	char * result_position = result;
	const size_t needle_start_distance = needle_position - haystack;
	strncpy(result_position, haystack_position, needle_start_distance);
	haystack_position += needle_start_distance;
	result_position += needle_start_distance;
	strncpy(result_position, replacement, replacement_length);
	haystack_position += needle_length;
	result_position += replacement_length;
	const size_t needle_end_distance = result_position - result;
	strncpy(result_position, haystack_position, result_size - needle_end_distance);
	return result;
}

/**
Converts a string to a standard stream.

@param str The string to convert.
@return The standard stream if the string is valid and <code>SUBNULL</code> otherwise.
**/
FILE * stdstr(const char * str) {
	if (strcmp(str, "stdin") == 0) return stdin;
	if (strcmp(str, "stdout") == 0) return stdout;
	if (strcmp(str, "stderr") == 0) return stderr;
	return NULL;
}

#endif
