/**
Provides general-purpose functions.

@file util.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#include <stdlib.h>//*alloc, free, size_t, NULL
#include <stdio.h>//*open, *close, *read, *write, *print*, std*, FILE
#include <string.h>//str*
#include <ctype.h>//is*
#include <unistd.h>//*page*

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
Returns the string length of an unsigned integer.

@param x The integer.
@return The string length.
**/
size_t uintlen(unsigned int x) {
	size_t len = 1;
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
int hash(const unsigned char * const array, const size_t size) {
	const int prime = 31;
	int result = 1;
	for (size_t byte = 0; byte < size; byte++) {
		result = prime * result + array[byte];
	}
	return result;
}

/**
Replaces special characters in a string with escape codes.

Allocates enough memory for the return value.

@param str The string to escape.
@return The resulting string if successful and <code>NULL</code> otherwise.
**/
char * astresc(const char * const str) {
	if (str == NULL) {
		return NULL;
	}
	const size_t size = 4 * strlen(str) + 1;
	char * const result = malloc(size);
	if (result == NULL) {
		return NULL;
	}
	const char * str_position = str;
	char * result_position = result;
	while (*str_position != '\0') {
		const unsigned char byte = (unsigned char )*str_position;
		switch (*str_position) {
			case '\a':
				*result_position++ = '\\';
				*result_position++ = 'a';
				break;
			case '\b':
				*result_position++ = '\\';
				*result_position++ = 'b';
				break;
			case '\t':
				*result_position++ = '\\';
				*result_position++ = 't';
				break;
			case '\n':
				*result_position++ = '\\';
				*result_position++ = 'n';
				break;
			case '\v':
				*result_position++ = '\\';
				*result_position++ = 'v';
				break;
			case '\f':
				*result_position++ = '\\';
				*result_position++ = 'f';
				break;
			case '\r':
				*result_position++ = '\\';
				*result_position++ = 'r';
				break;
			case '"':
				*result_position++ = '\\';
				*result_position++ = '"';
				break;
			/*case '\'':
				*result_position++ = '\\';
				*result_position++ = '\'';
				break;
			case '?':
				*result_position++ = '\\';
				*result_position++ = '?';
				break;*/
			case '\\':
				*result_position++ = '\\';
				*result_position++ = '\\';
				break;
			default:
				if (isprint(byte)) {
					*result_position++ = *str_position;
				}
				else {
					const char hex[16] = "0123456789abcdef";
					*result_position++ = '\\';
					*result_position++ = 'x';
					*result_position++ = hex[byte >> 0 & 0xf];
					*result_position++ = hex[byte >> 4 & 0xf];
				}
		}
		str_position++;
	}
	*result_position = '\0';
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
@return The resulting string if successful and <code>NULL</code> otherwise.
**/
char * astrrep(const char * const haystack, const char * const needle, const char * const replacement) {
	if (haystack == NULL) {
		return NULL;
	}
	const size_t haystack_size = strlen(haystack) + 1;
	const char * needle_position = NULL;
	if (needle != NULL && replacement != NULL) {
		needle_position = strstr(haystack, needle);
	}
	if (needle_position == NULL) {
		char * const result = malloc(haystack_size);
		if (result == NULL) {
			return NULL;
		}
		strcpy(result, haystack);
		return result;
	}
	const size_t needle_length = strlen(needle);
	const size_t replacement_length = strlen(replacement);
	const size_t result_size = haystack_size - needle_length + replacement_length;
	char * const result = malloc(result_size);
	if (result == NULL) {
		return NULL;
	}
	const char * haystack_position = haystack;
	char * result_position = result;
	const size_t needle_start_distance = (size_t )(needle_position - haystack);
	strncpy(result_position, haystack_position, needle_start_distance);
	haystack_position += needle_start_distance;
	result_position += needle_start_distance;
	strncpy(result_position, replacement, replacement_length);
	haystack_position += needle_length;
	result_position += replacement_length;
	const size_t needle_end_distance = (size_t )(result_position - result);
	strncpy(result_position, haystack_position, result_size - needle_end_distance);
	return result;
}

/**
Copies a file.

@param dest The destination.
@param src The source.
@return 0 if successful and -1 otherwise.
**/
int copy(const char * const dest, const char * const src) {
	if (dest == NULL || src == NULL) {
		return -1;
	}
	FILE * const in = fopen(src, "rb");
	if (in == NULL) {
		return -1;
	}
	int result = 0;
	FILE * const out = fopen(dest, "wb");
	if (out == NULL) {
		result = -1;
		goto sleep;
	}
	const size_t size = (size_t )getpagesize();
	unsigned char * const buf = malloc(size);
	if (buf == NULL) {
		result = -1;
		goto hell;
	}
	while (TRUE) {
		const size_t bytes = fread(buf, 1, size, in);
		if (fwrite(buf, bytes, 1, out) != 1) {
			if (feof(in) != 0) {
				result = 0;
			}
			else {
				result = -1;
			}
			break;
		}
	}
	free(buf);
	hell: if (fclose(out) == EOF) {
		result = -1;
	}
	sleep: if (fclose(in) == EOF) {
		result = -1;
	}
	return result;
}

/**
Copies a file if the destination is older than the source.

@param dest The destination.
@param src The source.
@return 0 if successful and -1 otherwise.
**/
int smart_copy(const char * const dest, const char * const src) {
	return copy(dest, src);//TODO implement
}

/**
Converts a string to a standard stream.

@param str The string to convert.
@return The standard stream if the string is valid and <code>NULL</code> otherwise.
**/
FILE * stdstr(const char * const str) {
	if (str == NULL) {
		return NULL;
	}
	if (strcmp(str, "stdin") == 0) return stdin;
	if (strcmp(str, "stdout") == 0) return stdout;
	if (strcmp(str, "stderr") == 0) return stderr;
	return NULL;
}
