/**
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef UTIL_C
#define UTIL_C

#include <stdlib.h>//*alloc, size_t, NULL
#include <stdio.h>//*print*, std*, FILE
#include <string.h>//str*

#include "util.h"//SUBNULL

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
Replaces string in a string with a string.

Allocates enough memory for the return value.

@param haystack The string to search from.
@param needle The string to search for.
@param replacement The string to replace with.
@return The resulting string.
**/
int strnchr(char *input, char target) {
	int output = 0;
	while (input = strchr(input, target)) {
		output++;
		input++;
	}
	return output;
}
int strnstr(char *input, char *target) {
	int output = 0;
	int length = strlen(target);
	while (input = strstr(input, target)) {
		output++;
		input += length;
	}
	return output;
}
char * astrrep(char *input, char *source, char *target) {//(const char * haystack, const char * needle, const char * replacement) {
	int input_length = strlen(input)+1;
	int source_length = strlen(source);
	int target_length = strlen(target);
	char *output = malloc((input_length+strnstr(input, source)*(target_length-source_length))*sizeof (char));
	int input_offset = 0;
	int output_offset = 0;
	char *pointer;
	while (pointer = strstr(input+input_offset, source)) {
		int output_length = pointer-(input+input_offset);
		strncpy(output+output_offset, input+input_offset, output_length);
		input_offset += output_length;
		output_offset += output_length;
		strncpy(output+output_offset, target, target_length);
		input_offset += source_length;
		output_offset += target_length;
	}
	strncpy(output+output_offset, input+input_offset, input_length-input_offset);
	return output;
	//return haystack;
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
		result = prime*result+array[index];
		index++;
	}
	return result;
}

/**
Converts a string to a standard stream.

@param str The string to convert.
@return The standard stream if the string is valid and <code>SUBNULL</code> otherwise.
**/
FILE * stdstr(const char * str) {
	if (strcmp(str, "null") == 0) return NULL;
	if (strcmp(str, "stdin") == 0) return stdin;
	if (strcmp(str, "stdout") == 0) return stdout;
	if (strcmp(str, "stderr") == 0) return stderr;
	return SUBNULL;
}

#endif
