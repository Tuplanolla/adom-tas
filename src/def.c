/**
Sets the default values of various variables.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef DEF_C
#define DEF_C

#include <stddef.h>//size_t

#include <curses.h>//KEY_*

#include "util.h"//intern, bool, FALSE, TRUE

intern const char * const project_name = "adom-tas";
intern const char * const project_version = "1.0.0";

intern const char * const interface_left_more = "<-";
intern const char * const interface_right_more = "->";
intern const char * const interface_left_used = "[";
intern const char * const interface_right_used = "]";
intern const char * const interface_left_unused = "";
intern const char * const interface_right_unused = "";
intern const int interface_colors[8] = {
	COLOR_BLACK,
	COLOR_WHITE,
	COLOR_RED,
	COLOR_YELLOW,
	COLOR_GREEN,
	COLOR_CYAN,
	COLOR_BLUE,
	COLOR_MAGENTA
};

intern const char * const log_separator = " - ";
intern const char * const log_error = "Error";
intern const char * const log_warning = "Warning";
intern const char * const log_note = "Note";
intern const char * const log_call = "Call";

intern const char * const default_executable_path = "adom/adom";
intern const char * const default_loader_path = "bin/adom-tas.so";
intern const char * const default_libc_path = "/lib/libc.so.6";
intern const char * const default_libncurses_path = "/usr/lib/libncurses.so.5";
intern const int default_states = 9;
intern const int default_rows = 25;
intern const int default_cols = 77;
intern const char * const default_shm_path = "adom-tas.shm";
intern const int default_timestamp = 0;
intern const int default_generations = 100;
intern const int default_sql = TRUE;
intern const int default_autoplay = FALSE;
intern const int default_color = TRUE;
intern const char * const default_iterator = "#";
intern const char * const default_input_path = "input.tas";
intern const char * const default_output_path = "output.tas";
intern const char * const default_error_stream = "stderr";
intern const char * const default_warning_stream = "stderr";
intern const char * const default_note_stream = "stderr";
intern const char * const default_call_stream = "/dev/null";
intern const int default_save_key = KEY_F(2);
intern const int default_load_key = KEY_F(3);
intern const int default_state_key = KEY_F(8);
intern const int default_unstate_key = MOD_SHIFT + KEY_F(8);
intern const int default_duration_key = KEY_F(6);
intern const int default_unduration_key = MOD_SHIFT + KEY_F(6);
intern const int default_time_key = KEY_F(5);
intern const int default_untime_key = MOD_SHIFT + KEY_F(5);
intern const int default_menu_key = KEY_F(9);
intern const int default_condense_key = KEY_F(10);
intern const int default_hide_key = MOD_SHIFT + KEY_F(10);
intern const int default_play_key = KEY_F(11);
intern const int default_stop_key = MOD_SHIFT + KEY_F(11);
intern const int default_quit_key = MOD_SHIFT + KEY_F(12);

intern const char * const default_config_path = "adom-tas.cfg";
intern const char * const default_config =
	"#required\n"
	"executable     = \"adom/adom\"\n"
	"data           = \"~/.adom.data\"\n"
	"loader         = \"bin/adom-tas.so\"\n"
	"libc           = \"/lib/libc.so.6\"\n"
	"libncurses     = \"/usr/lib/libncurses.so.5\"\n"
	"\n"
	"#optional\n"
	"#home           = \"/home/user\"\n"
	"#states         = 4\n"
	"#rows           = 25\n"
	"#cols           = 77\n"
	"#shm            = \"adom-tas.shm\"\n"
	"#generations    = 100\n"
	"#timestamp      = 0\n"
	"#sql            = true\n"
	"#autoplay       = false\n"
	"#color          = true\n"
	"#iterator       = \"#\"\n"
	"#input          = \"input.tas\"\n"
	"#output         = \"output.#.tas\"\n"
	"#error          = \"adom-tas.log\"\n"
	"#warning        = \"adom-tas.log\"\n"
	"#note           = \"adom-tas.log\"\n"
	"#call           = \"/dev/null\"\n"
	"#save_key       = 266#F2\n"
	"#load_key       = 267#F3\n"
	"#state_key      = 272#F8\n"
	"#unstate_key    = 284#Shift F8\n"
	"#duration_key   = 270#F6\n"
	"#unduration_key = 282#Shift F6\n"
	"#time_key       = 269#F5\n"
	"#untime_key     = 281#Shift F5\n"
	"#menu_key       = 273#F9\n"
	"#condense_key   = 274#F10\n"
	"#hide_key       = 286#Shift F10\n"
	"#play_key       = 275#F11\n"
	"#stop_key       = 287#Shift F11\n"
	"#quit_key       = 288#Shift F12\n"
;

#endif
