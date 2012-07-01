/**
Manages the configuration variables.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>//FILE

#include "problem.h"//problem_t
#include "record.h"//record_t

/**
The configuration variables.
**/
extern char * home_path;
extern char * executable_path;
extern char * executable_data_path;
extern char * executable_temporary_path;
extern char * executable_config_path;
extern char * executable_process_path;
extern char * executable_keybind_path;
extern char * executable_version_path;
extern char * executable_count_path;
extern char * loader_path;
extern char * libc_path;
extern char * libncurses_path;
extern unsigned int states;
extern unsigned int rows;
extern unsigned int cols;
extern char * shm_path;
extern unsigned int generations;
extern bool sql;
extern bool autoplay;
extern bool color;
extern char * iterator;
extern FILE * input_stream;
extern FILE ** output_streams;
extern FILE * error_stream;
extern FILE * warning_stream;
extern FILE * note_stream;
extern FILE * call_stream;
extern int time_key;
extern int untime_key;
extern int save_key;
extern int load_key;
extern int state_key;
extern int unstate_key;
extern int menu_key;
extern int unmenu_key;
extern int play_key;
extern int quit_key;

/**
The configuration functions.
**/
problem_t init_launcher_config(void);
problem_t init_loader_config(void);
problem_t uninit_config(void);

#endif
