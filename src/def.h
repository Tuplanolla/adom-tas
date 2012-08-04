/**
@see def.c
@author Sampsa "Tuplanolla" Kiiskinen

TODO fix execution modes
**/
#ifndef DEF_H
#define DEF_H

#include <stddef.h>//size_t

/**
The execution modes.
**/
enum mode_e {
	GETTING_STARTED,
	ALMOST_READY,
	RUNNING,
	HAD_ENOUGH
};
typedef enum mode_e mode_d;

extern const char * const project_name;
extern const char * const project_version;

extern const char * const interface_left_more;
extern const char * const interface_right_more;
extern const char * const interface_left_used;
extern const char * const interface_right_used;
extern const char * const interface_left_unused;
extern const char * const interface_right_unused;
extern const int interface_colors[6];

extern const char * const log_separator;
extern const char * const log_error;
extern const char * const log_warning;
extern const char * const log_note;
extern const char * const log_call;

extern const char * const default_exec_path;
extern const char * const default_loader_path;
extern const char * const default_libc_path;
extern const char * const default_libncurses_path;
extern const int default_enforce;
extern const int default_generations;
extern const int default_states;
extern const int default_rows;
extern const int default_cols;
extern const char * const default_shm_path;
extern const int default_timestamp;
extern const int default_generations;
extern const int default_sql;
extern const int default_preserve;//TODO rename
extern const int default_autoplay;
extern const int default_monochrome;
extern const char * const default_iterator;
extern const char * const default_input_path;
extern const char * const default_output_path;
extern const char * const default_error_stream;
extern const char * const default_warning_stream;
extern const char * const default_note_stream;
extern const char * const default_call_stream;
extern const int default_save_key;
extern const int default_load_key;
extern const int default_state_key;
extern const int default_unstate_key;
extern const int default_duration_key;
extern const int default_unduration_key;
extern const int default_time_key;
extern const int default_untime_key;
extern const int default_menu_key;
extern const int default_info_key;
extern const int default_condense_key;
extern const int default_hide_key;
extern const int default_play_key;
extern const int default_stop_key;
extern const int default_quit_key;

extern const char * const default_config_path;
extern const char * const default_config;

extern const char * const exec_config;//TODO rename
extern const char * const exec_keybind;

#endif
