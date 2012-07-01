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
extern unsigned int generations;
extern unsigned int states;
extern unsigned int rows;
extern unsigned int cols;
extern char * iterator;
extern FILE * input_stream;
extern FILE ** output_streams;
extern char * shm_path;
extern FILE * error_stream;
extern FILE * warning_stream;
extern FILE * note_stream;
extern FILE * call_stream;
extern record_t record;
extern bool autoplay;

/**
The configuration functions.
**/
problem_t init_launcher_config(void);
problem_t init_loader_config(void);
problem_t uninit_config(void);

#endif
