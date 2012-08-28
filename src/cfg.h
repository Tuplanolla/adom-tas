/**
@see cfg.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef CFG_H
#define CFG_H

#include <stdio.h>//FILE
#include <time.h>//time_t

#include "util.h"//bool

extern char * cfg_home_path;
extern char * cfg_exec_path;
extern char * cfg_exec_data_path;
extern char * cfg_exec_temporary_path;
extern char ** cfg_exec_temporary_paths;
extern char * cfg_exec_config_path;
extern char * cfg_exec_process_path;
extern char * cfg_exec_keybind_path;
extern char * cfg_exec_version_path;
extern char * cfg_exec_error_path;
extern char * cfg_exec_count_path;
extern char * cfg_lib_path;
extern char * cfg_libc_path;
extern char * cfg_libncurses_path;
extern bool cfg_enforce;
extern int cfg_saves;
extern int cfg_rows;
extern int cfg_cols;
extern char * cfg_shm_path;
extern unsigned int cfg_generations;
extern time_t cfg_timestamp;
extern bool cfg_sql;
extern bool cfg_preserve;
extern bool cfg_autoplay;
extern bool cfg_monochrome;
extern char * cfg_iterator;
extern char * cfg_input_path;
extern char ** cfg_output_paths;
extern FILE * cfg_error_stream;
extern FILE * cfg_warning_stream;
extern FILE * cfg_notice_stream;
extern FILE * cfg_call_stream;
extern int cfg_save_key;
extern int cfg_load_key;
extern int cfg_state_key;
extern int cfg_unstate_key;
extern int cfg_duration_key;
extern int cfg_unduration_key;
extern int cfg_time_key;
extern int cfg_untime_key;
extern int cfg_menu_key;
extern int cfg_condense_key;
extern int cfg_hide_key;
extern int cfg_play_key;
extern int cfg_stop_key;
extern int cfg_quit_key;

int uninit_config(void);
int init_main_config(void);
int init_lib_config(void);

#endif
