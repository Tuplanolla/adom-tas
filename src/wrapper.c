/**
Serves as wrapper for the executable.
**/
#ifndef MAIN_C
#define MAIN_C

/*
Half of these are unnecessary.
*/
#include <curses.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <libgen.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "config.h"
#include "util.h"
#include "adom.h"
#include "loader.h"

/**
Prints an error message and returns its error code.
@param code The error code.
@return The error code.
**/
error_t propagate(const error_t code) {
	fprintf(stderr, "Error: %s\n", error_message(code));
	return code;
}

/**
Runs the executable.
@param argc The amount of command line arguments.
@param argv The command line arguments.
@return 0 if successfully and an error number otherwise.
**/
int main(int argc, char **argv) {
	/*
	Manages the environment variable HOME.
	*/
	const char *home = getenv("HOME");
	if (home == NULL) {
		if (setenv("HOME", HOME, TRUE)) {
			return propagate(SETENV_HOME_ERROR);
		}
	}

	/*
	Manages the environment variable LD_PRELOAD.
	*/
	const char *ld_preload = getenv("LD_PRELOAD");
	if (ld_preload == NULL) {
		if (setenv("LD_PRELOAD", LIBRARY_PATH, TRUE)) {
			return propagate(SETENV_LD_PRELOAD_ERROR);
		}
	}

	/*
	Conjures bacon.
	*/
	int result;
	struct stat file;
	result = stat(EXECUTABLE_PATH, &file);
	if (result != 0) return propagate(STAT_ERROR);
	if (file.st_mode & (S_ISUID | S_ISGID)) {
		return propagate(USER_ID_ERROR);
	}

	/*
	Uses file system heuristics to identify the executable.
	*/
	if (file.st_size != 2452608) return propagate(WRONG_SIZE_ERROR);
	FILE *_fhandle = fopen(VERSION_DATA_PATH, "r");
	if (_fhandle != NULL) {
		unsigned char desired_version[4];
		desired_version[0] = 1;
		desired_version[1] = 1;
		desired_version[2] = 1;
		desired_version[3] = 0;
		unsigned char version[4];
		fread(version, sizeof (unsigned char), sizeof (version), _fhandle);
		fclose(_fhandle);
		if (version != NULL) {
			if (memcmp(version, desired_version, 4) != 0) return propagate(WRONG_VERSION_ERROR);
		}
	}

	/*
	Launches the executable.
	*/
	argc--; argv++;//removes the name of this executable
	if (execvp(EXECUTABLE_PATH, argv) == 0) return NO_ERROR;//doesn't work
	return EXECUTION_ERROR;
}

#endif
