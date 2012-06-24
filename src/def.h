/**
Sets the default values of various variables.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef DEF_H
#define DEF_H

#include <stdlib.h>//size_t

#include "util.h"//bool

extern const char * const project_name;

extern const char * const log_separator;
extern const char * const log_error;
extern const char * const log_warning;
extern const char * const log_note;
extern const char * const log_call;

extern const char * const default_home_path;
extern const char * const default_executable_path;
extern const char * const default_loader_path;
extern const char * const default_libc_path;
extern const char * const default_libncurses_path;
extern const unsigned int default_generations;
extern const unsigned int default_states;
extern const unsigned int default_rows;
extern const unsigned int default_cols;
extern const char * const default_iterator;
extern const char * const default_input_path;
extern const char * const default_output_path;
extern const char * const default_shm_path;
extern const char * const default_error_stream;
extern const char * const default_warning_stream;
extern const char * const default_note_stream;
extern const char * const default_call_stream;
extern const bool default_play;
extern const char * const default_config_path;
extern const char * const default_config;

#endif
