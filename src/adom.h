/**
Provides information about
	the files,
	the variables and
	the behavior
		of the executable.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef ADOM_H
#define ADOM_H

#include <stdlib.h>//size_t

/**
The size of the executable.
**/
const size_t executable_size = 2452608;

/**
The hash code of the executable.
**/
const int executable_hash = 952942381;

/**
The version of the executable.
**/
const unsigned char executable_version[4] = {1, 1, 1, 0};

/**
The simulated turn count.

Ignores negative turns.
**/
unsigned int turns = 0;

/**
The turn count.
**/
unsigned int * const executable_turns = (void * )0x082b16e0;

/**
The save count.
**/
unsigned int * const executable_saves = (void * )0x082b6140;

/**
The minimum and maximum height and width of the terminal.
**/
const unsigned int executable_rows_min = 25;
const unsigned int executable_cols_min = 77;
const unsigned int executable_rows_max = 127;
const unsigned int executable_cols_max = 127;

/**
The data files of the executable.
**/
const char * const executable_data_file = ".adom.data";
const char * const executable_process_file = ".adom.prc";
const char * const executable_version_file = ".adom.ver";
const char * const executable_count_file = ".adom.cnt";
const char * const executable_keybind_file = ".adom.kbd";
const char * const executable_config_file = ".adom.cfg";

/**
The random number generator variables of the executable.
**/
unsigned char * const executable_arc4_s = (void * )0x082ada40;
unsigned char * const executable_arc4_i = (void * )0x082adb40;
unsigned char * const executable_arc4_j = (void * )0x082adb41;

/**
The amounts of random number generator calls measured in the executable.
**/
const unsigned int executable_arc4_calls_menu = 4 * 1214;
const unsigned int executable_arc4_calls_automatic_load = 4 * 1419;
const unsigned int executable_arc4_calls_manual_load = 4 * 1623;

/**
Simulates the random number generator of the executable.
**/
void sarc4(const int seed);
unsigned char arc4();
void iarc4(const int seed, const unsigned int bytes);

/**
Simulates the key number interpreter of the executable.
**/
void key_code(char * code, const int key);

#endif
