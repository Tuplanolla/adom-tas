/**
Contains the default values of configuration variables.
**/
#ifndef CONFIG_H
#define CONFIG_H

const unsigned int file_size = 2452608;
const unsigned char file_version[4] = {1, 1, 1, 0};
const char * const config_file = ".adom.cfg";
const char * const version_file = ".adom.ver";
const char * const keybind_file = ".adom.kbd";
const char * const process_file = ".adom.prc";
const char * const default_error_name = "stderr";
const char * const default_warning_name = "stderr";
const char * const default_note_name = "stderr";
const char * const default_call_name = "null";
const int default_rows = 25;
const int default_cols = 77;
const int default_states = 9;
const char * const default_shm_name = "adom-tas";
const char * const default_config_name = "adom-tas.cfg";
const char * const default_config_str =
		"#required\n"
		"exec       = \"/var/games/adom\"\n"
		"data       = \"/root/.adom.data\"\n"
		"libc       = \"/lib/libc.so.5\"\n"
		"libncurses = \"/usr/lib/libncurses.so.5\"\n"
		"ld_preload = \"/root/adom-tas/bin/adom-tas.so\"\n"
		"#optional\n"
		"shm        = /*/dev/shm*/\"/adom-tas\"\n"
		"input      = \"input.tas\"\n"
		"output     = \"output.tas\"\n"
		"error      = \"/var/log/adom-tas.log\"\n"
		"warning    = \"/var/log/adom-tas.log\"\n"
		"note       = \"/var/log/adom-tas.log\"\n"
		"call       = \"null\"\n"
		"rows       = 25\n"
		"cols       = 77\n"
		"states     = 9\n"
	;

/**
Sets the minimum and maximum amount of save states.
**/
#define STATES_MIN 1
#define STATES_MAX 256

/**
Sets the default locations of (all kinds of things).
**/
#define TEMPORARY_HOME "/home/user"
#define TEMPORARY_ACTUAL_PATH TEMPORARY_HOME"/adom-tas/actual_state.log"
#define TEMPORARY_SIM_PATH TEMPORARY_HOME"/adom-tas/simulated_state.log"

/**
Sets the size of the terminal.
**/
#define TERM_ROW 25
#define TERM_COL 80

#endif
