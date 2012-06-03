/**
Serves as wrapper for the executable.
**/
#ifndef WRAPPER_C
#define WRAPPER_C

/*
Annotated for later removal.
*/
#include <stdlib.h>//getenv setenv
#include <stdio.h>
#include <string.h>//memcmp
#include <sys/stat.h>//S_*
#include <unistd.h>//unlink execvp
#include <curses.h>

#include "config.h"
#include "util.h"
#include "adom.h"
#include "error.h"

/**
Prints a problem message and returns its error code.
@param code The error code.
@return The error code.
**/
error_t problem(const error_t code) {
	fprintf(stderr, "Problem: %s\n", error_message(code));
	return code;
}

/**
Runs the executable.
@param argc The amount of command line arguments.
@param argv The command line arguments.
@return The return code of the executable if successful and <code>EXECUTION_ERROR</code> otherwise.
**/
int main(int argc, char **argv) {
	/*
	Manages the environment variable HOME.
	*/
	const char *home = getenv("HOME");
	if (home == NULL) {
		if (setenv("HOME", HOME, TRUE)) {
			return problem(SETENV_HOME_ERROR);
		}
	}

	/*
	Manages the environment variable LD_PRELOAD.
	*/
	const char *ld_preload = getenv("LD_PRELOAD");
	if (ld_preload == NULL) {
		if (setenv("LD_PRELOAD", LIBRARY_PATH, TRUE)) {
			return problem(SETENV_LD_PRELOAD_ERROR);
		}
	}

	/*
	Conjures bacon.
	*/
	unlink(WHATEVER_DATA_PATH);
	int result;
	struct stat file;
	result = stat(EXECUTABLE_PATH, &file);
	if (result != 0) return problem(STAT_ERROR);
	if (file.st_mode & (S_ISUID | S_ISGID)) {
		return problem(USER_ID_ERROR);
	}

	/*
	Uses file system heuristics to identify the executable.
	*/
	if (file.st_size != 2452608) return problem(WRONG_SIZE_ERROR);
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
			if (memcmp(version, desired_version, 4) != 0) return problem(WRONG_VERSION_ERROR);
		}
	}

	/*
	Replaces the wrapper with the executable.
	*/
	argc--; argv++;//removes the name of the wrapper from the arguments
	if (execvp(EXECUTABLE_PATH, argv) == 0) return NO_ERROR;//never returns
	return EXECUTION_ERROR;
}

#endif
