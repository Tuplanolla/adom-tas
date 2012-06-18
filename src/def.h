/**
Contains the default values of various things.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef DEF_H
#define DEF_H

const char * const separator = " - ";
const char * const error_str = "Error";
const char * const warning_str = "Warning";
const char * const note_str = "Note";
const char * const call_str = "Call";
const unsigned char file_version[4] = {1, 1, 1, 0};
const size_t file_size = 2452608;
const int file_hash = 952942381;
const char * const config_file = ".adom.cfg";
const char * const version_file = ".adom.ver";
const char * const keybind_file = ".adom.kbd";
const char * const process_file = ".adom.prc";
const char * const default_error_name = "stderr";
const char * const default_warning_name = "stderr";
const char * const default_note_name = "stderr";
const char * const default_call_name = "null";
const char * const default_replacement = "#";
const int default_rows = 25;
const int default_cols = 77;
const int default_states = 9;
const char * const default_project_name = "adom-tas";
const char * const default_input_name = "input.tas";
const char * const default_output_name = "output.tas";
const char * const default_home_name = "/user/home";
const char * const default_ld_preload_name = "bin/adom-tas.so";
const char * const default_shm_name = "adom-tas.shm";
const char * const default_config_name = "adom-tas.cfg";
const char * const default_config_str =
		"#required\n"
		"executable = \"/var/games/adom\"\n"
		"data       = \"~/.adom.data\"\n"
		"loader     = \"bin/adom-tas.so\"\n"
		"libc       = \"/lib/libc.so.5\"\n"
		"libncurses = \"/usr/lib/libncurses.so.5\"\n"
		"#optional\n"
		"iterator   = \"#\"\n"
		"shm        = \"adom-tas.shm\"\n"
		"input      = \"input.tas\"\n"
		"output     = \"output.#.tas\"\n"
		"error      = \"/var/log/adom-tas.log\"\n"//TODO locks and automation
		"warning    = \"/var/log/adom-tas.log\"\n"
		"note       = \"/var/log/adom-tas.log\"\n"
		"call       = \"null\"\n"
		"rows       = 25\n"
		"cols       = 77\n"
		"states     = 9\n"
	;

/**
Sets the frame rate.

The shortest and longest possible frames are 1/16 and 16 seconds respectively.
**/
const unsigned char fps = 16;//sqrt(256)

#endif
