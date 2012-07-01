/**
Sets the default values of various variables.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef DEF_H
#define DEF_H

#include <stddef.h>//size_t

#include "util.h"//bool

extern const char * const project_name;

extern const char * const log_separator;
extern const char * const log_error;
extern const char * const log_warning;
extern const char * const log_note;
extern const char * const log_call;

extern const char * const default_executable_path;
extern const char * const default_loader_path;
extern const char * const default_libc_path;
extern const char * const default_libncurses_path;
extern const int default_states;
extern const int default_rows;
extern const int default_cols;
extern const char * const default_shm_path;
extern const int default_generations;
extern const int default_sql;
extern const int default_autoplay;
extern const int default_color;
extern const char * const default_iterator;
extern const char * const default_input_path;
extern const char * const default_output_path;
extern const char * const default_error_stream;
extern const char * const default_warning_stream;
extern const char * const default_note_stream;
extern const char * const default_call_stream;
extern const int default_time_key;
extern const int default_untime_key;
extern const int default_save_key;
extern const int default_load_key;
extern const int default_state_key;
extern const int default_unstate_key;
extern const int default_menu_key;
extern const int default_unmenu_key;
extern const int default_play_key;
extern const int default_quit_key;

#endif
