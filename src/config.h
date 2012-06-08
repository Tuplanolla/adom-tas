/**
Provides default values for configuration variables.
**/
#ifndef CONFIG_H
#define CONFIG_H

extern int rows;
extern int cols;

const char * const config_path = "adom-tas.cfg";

const char * const config_file = ".adom.cfg";
const char * const version_file = ".adom.ver";
const char * const keybind_file = ".adom.kbd";
const char * const process_file = ".adom.prc";

/**
Sets the default locations of (all kinds of things).
**/
#define TEMPORARY_HOME "/home/user"
#define TEMPORARY_INPUT_PATH TEMPORARY_HOME"/adom-tas/input.tas"
#define TEMPORARY_OUTPUT_PATH TEMPORARY_HOME"/adom-tas/output.tas"
#define TEMPORARY_ACTUAL_PATH TEMPORARY_HOME"/adom-tas/actual_state.log"
#define TEMPORARY_SIM_PATH TEMPORARY_HOME"/adom-tas/simulated_state.log"

/**
Sets the amount of save states.
**/
#define SAVE_STATES 10

/**
Sets the size of the terminal.
**/
#define TERM_ROW 25
#define TERM_COL 80

/**
Sets the default location of shared memory (in /dev/shm).
**/
#define SHM_PATH "/adom-tas"

/**
Sets the default locations of dynamically linked libraries.
**/
#define LIBC_PATH "/lib/libc.so.6"
#define LIBNCURSES_PATH "/usr/lib/libncurses.so.5"

#endif
