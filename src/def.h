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

extern const char * const def_exec_path;
extern const char * const def_lib_path;
extern const char * const def_libc_path;
extern const char * const def_libncurses_path;
extern const int def_enforce;
extern const int def_generations;
extern const int def_states;
extern const int def_rows;
extern const int def_cols;
extern const char * const def_shm_path;
extern const int def_timestamp;
extern const int def_generations;
extern const int def_sql;
extern const int def_preserve;
extern const int def_autoplay;
extern const int def_monochrome;
extern const char * const def_iterator;
extern const char * const def_input_path;
extern const char * const def_output_path;
extern const char * const def_error_stream;
extern const char * const def_warning_stream;
extern const char * const def_note_stream;
extern const char * const def_call_stream;
extern const int def_save_key;
extern const int def_load_key;
extern const int def_state_key;
extern const int def_unstate_key;
extern const int def_duration_key;
extern const int def_unduration_key;
extern const int def_time_key;
extern const int def_untime_key;
extern const int def_menu_key;
extern const int def_info_key;
extern const int def_condense_key;
extern const int def_hide_key;
extern const int def_play_key;
extern const int def_stop_key;
extern const int def_quit_key;

extern const char * const def_config_path;
extern const char * const def_config;

extern const char * const def_exec_config;
extern const char * const def_exec_keybind;

#endif
