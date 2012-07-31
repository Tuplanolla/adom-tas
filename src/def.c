/**
Sets the default values of various variables.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef DEF_C
#define DEF_C

#include <stddef.h>//size_t

#include <curses.h>//KEY_*

#include "util.h"//intern, bool, FALSE, TRUE

#include "def.h"

/**
The name of this project.

Used in unique identifier generation.
**/
intern const char * const project_name = "adom-tas";
/**
The release version of this project.
**/
intern const char * const project_version = "1.0.0";

/**
The indicator used when a list extends left.
**/
intern const char * const interface_left_more = "<-";
/**
The indicator used when a list extends right.
**/
intern const char * const interface_right_more = "->";
/**
The indicator used on the left side of a used field.
**/
intern const char * const interface_left_used = "[";
/**
The indicator used on the right side of a used field.
**/
intern const char * const interface_right_used = "]";
/**
The indicator used on the left side of an unused field.
**/
intern const char * const interface_left_unused = "";
/**
The indicator used on the right side of an unused field.
**/
intern const char * const interface_right_unused = "";
/**
The colors.

Ordered by hue (wavelength).
**/
intern const int interface_colors[6] = {
	COLOR_RED,
	COLOR_YELLOW,
	COLOR_GREEN,
	COLOR_CYAN,
	COLOR_BLUE,
	COLOR_MAGENTA
};

/**
The separator between error message fields.
**/
intern const char * const log_separator = " - ";
/**
The error string.
**/
intern const char * const log_error = "Error";
/**
The warning string.
**/
intern const char * const log_warning = "Warning";
/**
The note string.
**/
intern const char * const log_note = "Note";
/**
The library call string.
**/
intern const char * const log_call = "Call";

/*
Undocumented.
*/
intern const char * const default_exec_path = "adom/adom";
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
intern const int default_duration_key = KEY_F(5);
intern const int default_unduration_key = MOD_SHIFT + KEY_F(5);
intern const int default_time_key = KEY_F(6);
intern const int default_untime_key = MOD_SHIFT + KEY_F(6);
intern const int default_menu_key = KEY_F(9);
intern const int default_condense_key = KEY_F(10);
intern const int default_hide_key = MOD_SHIFT + KEY_F(10);
intern const int default_play_key = KEY_F(11);
intern const int default_stop_key = MOD_SHIFT + KEY_F(11);
intern const int default_quit_key = MOD_SHIFT + KEY_F(12);

/**
The location of the configuration file.
**/
intern const char * const default_config_path = "adom-tas.cfg";
/**
The template configuration.
**/
intern const char * const default_config =
	"#required\n"
	"executable     = \"adom/adom\"\n"
	"data           = \"~/.adom.data\"\n"
	"loader         = \"bin/adom-tas.so\"\n"
	"libc           = \"/lib/libc.so.6\"\n"
	"libncurses     = \"/usr/lib/libncurses.so.5\"\n"
	"\n"
	"#optional\n"
	"#enforce        = false\n"
	"#home           = \"/home/user\"\n"
	"#states         = 4\n"
	"#rows           = 25\n"
	"#cols           = 77\n"
	"#shm            = \"adom-tas.shm\"\n"
	"#generations    = 100\n"
	"#timestamp      = 189284349\n"
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
	"#duration_key   = 270#F5\n"
	"#unduration_key = 282#Shift F5\n"
	"#time_key       = 269#F6\n"
	"#untime_key     = 281#Shift F6\n"
	"#menu_key       = 273#F9\n"
	"#condense_key   = 274#F10\n"
	"#hide_key       = 286#Shift F10\n"
	"#play_key       = 275#F11\n"
	"#stop_key       = 287#Shift F11\n"
	"#quit_key       = 288#Shift F12\n"
;

#endif
