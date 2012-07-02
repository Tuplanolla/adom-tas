/**
Simulates
	the random number generator and
	the key number interpreter
		of the executable.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef EXEC_C
#define EXEC_C

#include <stdlib.h>//*random, size_t
#include <stdio.h>//*print*
#include <string.h>//str*, mem*

#include <curses.h>//KEY_*

#include "util.h"//intern, SWAP

#include "exec.h"//executable_*

/**
The actual turn count.

Ignores negative turns.
**/
intern int turns = 0;

/**
The size of the executable.
**/
intern const size_t executable_size = 2452608;

/**
The hash code of the executable.
**/
intern const int executable_hash = 952942381;

/**
The version of the executable.
**/
intern const unsigned char executable_version[4] = {1, 1, 1, 0};

/**
A pointer to the turn count.
**/
intern int * const executable_turns = (void * )0x082b16e0;

/**
A pointer to the save count.
**/
intern int * const executable_saves = (void * )0x082b6140;

/**
The minimum and maximum height and width of the terminal.
**/
intern const int executable_rows_min = 25;
intern const int executable_cols_min = 77;
intern const int executable_rows_max = 127;
intern const int executable_cols_max = 127;

/**
The data files of the executable.
**/
intern const char * const executable_data_directory = ".adom.data";
intern const char * const executable_temporary_directory = "tmpdat";
intern const char * const executable_temporary_file = "adom_tdl";
intern const unsigned int executable_temporary_levels = 51;
intern const unsigned int executable_temporary_parts = 4;
intern const char * const executable_config_file = ".adom.cfg";
intern const char * const executable_process_file = ".adom.prc";
intern const char * const executable_keybind_file = ".adom.kbd";
intern const char * const executable_version_file = ".adom.ver";
intern const char * const executable_count_file = ".adom.cnt";

/**
The random number generator variables of the executable.
**/
intern unsigned char * const executable_arc4_s = (void * )0x082ada40;
intern unsigned char * const executable_arc4_i = (void * )0x082adb40;
intern unsigned char * const executable_arc4_j = (void * )0x082adb41;

/**
The amounts of random number generator calls measured in the executable.
**/
intern const unsigned int executable_arc4_calls_menu = 4 * 1214;
intern const unsigned int executable_arc4_calls_automatic_load = 4 * 1419;
intern const unsigned int executable_arc4_calls_manual_load = 4 * 1623;

/**
The current state S and iterators i and j of the random number generator.
**/
intern unsigned char arc4_s[0x100];
intern unsigned char arc4_i = 0x00;
intern unsigned char arc4_j = 0x00;

/**
Seeds the current state S.

@param seed The seed k.
**/
void sarc4(const int seed) {
	unsigned char i = 0x00, j = 0x00;
	do {
		arc4_s[i] = i;
		i++;
	} while(i != 0x00);
	do {
		j = (unsigned char )(j + (arc4_s[i] + ((const unsigned char * )&seed)[i % sizeof seed]));
		SWAP(arc4_s[i], arc4_s[j]);
		i++;
	} while (i != 0x00);
}

/**
Generates a byte r (and changes the current state).

The order of operations is wrong to replicate the behavior of the executable.

@return The byte r.
**/
unsigned char arc4(void) {
	arc4_j = (unsigned char )(arc4_j + arc4_s[arc4_i]);//should be at point A
	SWAP(arc4_s[arc4_i], arc4_s[arc4_j]);//should be at point B
	arc4_i++;
	//point A
	//point B
	return arc4_s[(unsigned char )(arc4_s[arc4_i] + arc4_s[arc4_j])];
}

/**
Generates and injects
	the state S and
	the iterators i and j.

@param seed The seed k to use.
@param bytes The amount of bytes r to generate.
**/
void iarc4(const unsigned int seed, const unsigned int bytes) {
	arc4_i = 0;
	arc4_j = 0;
	srandom(seed);
	sarc4(random());
	for (unsigned int byte = 0; byte < bytes; byte++) {
		arc4();
	}
	memcpy(executable_arc4_s, arc4_s, sizeof arc4_s);
	memcpy(executable_arc4_i, &arc4_i, sizeof arc4_i);
	memcpy(executable_arc4_j, &arc4_j, sizeof arc4_j);
	(*executable_saves)++;
}

/*
Returns the key code of a key number.

The code is generated automatically:
<pre>
gcc src/meta.c -O3 -o obj/meta
mkdir -p src/meta
obj/meta key_code > src/meta/key_code.c
rm -f obj/meta
</pre>

@param code The key number.
@return The key code.
*/
#include "meta/key_code.c"

#endif
