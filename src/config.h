/**
Provides default values for configuration variables.
**/
#ifndef CONFIG_H
#define CONFIG_H

unsigned int file_size = 2452608;
unsigned char file_version[4] = {1, 1, 1, 0};
const char * const config_file = ".adom.cfg";
const char * const version_file = ".adom.ver";
const char * const keybind_file = ".adom.kbd";
const char * const process_file = ".adom.prc";

const char * const config_path = "adom-tas.cfg";
const char * const config_str = 
		"exec       = \"/var/games/adom\"//required"
		"data       = \"/root/.adom.data\"//required"
		"libc       = \"/lib/libc.so.5\"//required"
		"libncurses = \"/usr/lib/libncurses.so.5\"//required"
		"ld_preload = \"/root/adom-tas/bin/adom-tas.so\"//required"
		"input      = \"/root/adom-tas/input.tas\""
		"output     = \"/root/adom-tas/output.tas\""
		"error      = \"/var/log/adom-tas.log\""
		"warning    = \"/var/log/adom-tas.log\""
		"note       = \"/var/log/adom-tas.log\""
		"call       = \"/dev/null\""
		"states     = 10"
		"rows       = 25"
		"cols       = 80"
	;

/**
Sets the default locations of (all kinds of things).
**/
#define TEMPORARY_HOME "/home/user"
#define TEMPORARY_ACTUAL_PATH TEMPORARY_HOME"/adom-tas/actual_state.log"
#define TEMPORARY_SIM_PATH TEMPORARY_HOME"/adom-tas/simulated_state.log"

/**
Sets the default location of shared memory (in /dev/shm).
**/
#define SHM_PATH "/adom-tas"

/**
Sets the amount of save states.
**/
#define SAVE_STATES 10

/**
Sets the size of the terminal.
**/
#define TERM_ROW 25
#define TERM_COL 80

#endif
