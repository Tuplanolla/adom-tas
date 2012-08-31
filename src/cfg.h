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
extern bool cfg_force_sync;
extern int cfg_saves;
extern int cfg_rows;
extern int cfg_cols;
extern char * cfg_shm_path;
extern unsigned int cfg_generations;
extern time_t cfg_timestamp;
extern bool cfg_emulate_sql;
extern bool cfg_keep_saves;
extern bool cfg_play_instantly;
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
extern int cfg_next_save_key;
extern int cfg_prev_save_key;
extern int cfg_longer_duration_key;
extern int cfg_shorter_duration_key;
extern int cfg_more_time_key;
extern int cfg_less_time_key;
extern int cfg_menu_key;
extern int cfg_info_key;
extern int cfg_condense_key;
extern int cfg_hide_key;
extern int cfg_play_key;
extern int cfg_stop_key;
extern int cfg_quit_key;

int cfg_uninit(void);
int cfg_init_main(void);
int cfg_init_lib(void);

#endif
