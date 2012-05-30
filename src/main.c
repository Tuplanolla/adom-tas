/**
Provides.
**/
#ifndef MAIN_C
#define MAIN_C
/**
This is a mess, but works, which is enough for the first build.
**/

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
#include "adom-tas.h"

/**
Tests the class.
@param argc The amount of command line arguments.
@param argv The command line arguments.
@return 0 if successful and an error number otherwise.
**/
const int main(const int argc, const char **argv) {
	getenv("HOME");

	/*
	Manages the environment variable LD_PRELOAD.
	*/
	const char *preload = getenv("LD_PRELOAD");
	if (preload == NULL) {
		if (setenv("LD_PRELOAD", LIBRARY_PATH, TRUE)) {
			return propagate(SETENV_ERROR);
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
	if (file.st_size != 2452608) return propagate(WRONG_SIZE_ERROR);

	/*
	Launches the executable.
	*/
	const char *adom_argv = argv+1;//TODO explain
	const int adom_argc = argc-1;
	execvp(EXECUTABLE_PATH, adom_argv);//execute with varargs including PATH

	return 0;
}

#endif
