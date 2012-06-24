/**
Sets the default values of various variables.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef DEF_C
#define DEF_C

#include <stdlib.h>//size_t

#include "util.h"//bool, FALSE, TRUE

const char * const project_name = "adom-tas";

const char * const log_separator = " - ";
const char * const log_error = "Error";
const char * const log_warning = "Warning";
const char * const log_note = "Note";
const char * const log_call = "Call";

const char * const default_home_path = "/user/home";
const char * const default_executable_path = "adom/adom";
const char * const default_loader_path = "bin/adom-tas.so";
const char * const default_libc_path = "/lib/libc.so.6";
const char * const default_libncurses_path = "/usr/lib/libncurses.so.5";
const unsigned int default_generations = 100;
const unsigned int default_states = 9;
const unsigned int default_rows = 25;
const unsigned int default_cols = 77;
const char * const default_iterator = "#";
const char * const default_input_path = "input.tas";
const char * const default_output_path = "output.tas";
const char * const default_shm_path = "adom-tas.shm";
const char * const default_error_stream = "stderr";
const char * const default_warning_stream = "stderr";
const char * const default_note_stream = "stderr";
const char * const default_call_stream = "null";
const bool default_play = FALSE;
const char * const default_config_path = "adom-tas.cfg";
const char * const default_config =
		"#required\n"
		"executable   = \"adom/adom\"\n"
		"data         = \"~/.adom.data\"\n"
		"loader       = \"bin/adom-tas.so\"\n"
		"libc         = \"/lib/libc.so.6\"\n"
		"libncurses   = \"/usr/lib/libncurses.so.5\"\n"
		"\n"
		"#optional\n"
		"#home        = \"/home/user\"\n"
		"#generations = 100\n"
		"#states      = 6\n"
		"#rows        = 25\n"
		"#cols        = 77\n"
		"#iterator    = \"(state)\"\n"
		"#input       = \"input.tas\"\n"
		"#output      = \"output.(state).tas\"\n"
		"#shm         = \".adom-tas.shm\"\n"
		"#error       = \"adom-tas.log\"\n"//TODO locks and automation
		"#warning     = \"adom-tas.log\"\n"
		"#note        = \"adom-tas.log\"\n"
		"#call        = \"null\"\n"
		"#play        = false\n"
	;

#endif
