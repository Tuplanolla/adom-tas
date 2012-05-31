/**
Provides default values for configuration variables.
**/
#ifndef CONFIG_H
#define CONFIG_H

/**
Sets the default locations of (all kinds of things).
**/
#define HOME "/home/tuplanolla"//TODO automate
#define EXECUTABLE_PATH HOME"/adom/adom"
#define LIBRARY_PATH HOME"/adom-tas/bin/adom-tas.so"
#define LOG_PATH HOME"/adom-tas/all.log"
#define DATA_PATH HOME"/.adom.data"
#define VERSION_DATA_PATH DATA_PATH"/.adom.ver"
#define KEYBIND_DATA_PATH DATA_PATH"/.adom.kbd"
#define CONFIG_DATA_PATH DATA_PATH"/.adom.cfg"

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
Sets the default location of shared memory.
**/
#define SHM_PATH "/dev/shm/adom-tas"

/**
Sets the default locations of dynamically linked libraries.
**/
#define LIBC_PATH "/lib/libc.so.6"
#define LIBNCURSES_PATH "/lib/libncurses.so.5"

#endif
