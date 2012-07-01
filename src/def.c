/**
Sets the default values of various variables.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef DEF_C
#define DEF_C

#include <stddef.h>//size_t

#include "util.h"//intern, bool, FALSE, TRUE

intern const char * const project_name = "adom-tas";

intern const char * const log_separator = " - ";
intern const char * const log_error = "Error";
intern const char * const log_warning = "Warning";
intern const char * const log_note = "Note";
intern const char * const log_call = "Call";

intern const char * const default_executable_path = "adom/adom";
intern const char * const default_loader_path = "bin/adom-tas.so";
intern const char * const default_libc_path = "/lib/libc.so.6";
intern const char * const default_libncurses_path = "/usr/lib/libncurses.so.5";
intern const int default_generations = 100;
intern const int default_states = 9;
intern const int default_rows = 25;
intern const int default_cols = 77;
intern const char * const default_iterator = "#";
intern const char * const default_input_path = "input.tas";
intern const char * const default_output_path = "output.tas";
intern const char * const default_shm_path = "adom-tas.shm";
intern const char * const default_error_stream = "stderr";
intern const char * const default_warning_stream = "stderr";
intern const char * const default_note_stream = "stderr";
intern const char * const default_call_stream = "/dev/null";
intern const bool default_autoplay = FALSE;
intern const char * const default_config_path = "adom-tas.cfg";
intern const char * const default_config =
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
		"#shm         = \"adom-tas.shm\"\n"
		"#error       = \"adom-tas.log\"\n"
		"#warning     = \"adom-tas.log\"\n"
		"#note        = \"adom-tas.log\"\n"
		"#call        = \"/dev/null\"\n"
		"#autoplay    = false\n"
	;

#endif
