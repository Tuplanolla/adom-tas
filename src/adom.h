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
The simulated turn count.

Ignores negative turns.
**/
extern unsigned int turns;

static const size_t executable_size;

extern const int executable_hash;
extern const unsigned char executable_version[4];

extern unsigned int * const executable_turns;
extern unsigned int * const executable_saves;

extern const unsigned int executable_rows_min;
extern const unsigned int executable_cols_min;
extern const unsigned int executable_rows_max;
extern const unsigned int executable_cols_max;

extern const char * const executable_data_file;
extern const char * const executable_process_file;
extern const char * const executable_version_file;
extern const char * const executable_count_file;
extern const char * const executable_keybind_file;
extern const char * const executable_config_file;

extern unsigned char * const executable_arc4_s;
extern unsigned char * const executable_arc4_i;
extern unsigned char * const executable_arc4_j;

extern const unsigned int executable_arc4_calls_menu;
extern const unsigned int executable_arc4_calls_automatic_load;
extern const unsigned int executable_arc4_calls_manual_load;

/**
Simulates the random number generator of the executable.
**/
void sarc4(const int seed);
unsigned char arc4(void);
void iarc4(const int seed, const unsigned int bytes);

/**
Simulates the key number interpreter of the executable.
**/
void key_code(char * code, const int key);

#endif
