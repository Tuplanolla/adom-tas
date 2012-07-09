/**
@see exec.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef EXEC_H
#define EXEC_H

#include <stddef.h>//size_t

extern short pairs;
extern int turns;

extern const size_t executable_size;
extern const int executable_hash;
extern const unsigned char executable_version[4];

extern const int executable_rows_min;
extern const int executable_rows_max;
extern const int executable_cols_min;
extern const int executable_cols_max;

extern const char * const executable_data_directory;
extern const char * const executable_temporary_directory;
extern const char * const executable_temporary_file;
extern const unsigned int executable_temporary_levels;
extern const unsigned int executable_temporary_parts;
extern const char * const executable_config_file;
extern const char * const executable_process_file;
extern const char * const executable_keybind_file;
extern const char * const executable_version_file;
extern const char * const executable_count_file;

extern const char * const executable_config;
extern const char * const executable_keybind;

extern int * const executable_turns;
extern int * const executable_saves;

extern unsigned char * const executable_arc4_s;
extern unsigned char * const executable_arc4_i;
extern unsigned char * const executable_arc4_j;

extern const unsigned int executable_arc4_calls_menu;
extern const unsigned int executable_arc4_calls_automatic_load;
extern const unsigned int executable_arc4_calls_manual_load;

void sarc4(int seed);
unsigned char arc4(void);
void iarc4(unsigned int seed, unsigned int bytes);
const char * key_code(int key);

#endif
