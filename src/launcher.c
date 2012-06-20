/**
Launches the executable.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef LAUNCHER_C
#define LAUNCHER_C

#include <stdlib.h>//*alloc, free, *env
#include <stdio.h>//*print*, *open, *close, std*, FILE
#include <string.h>//str*
#include <pwd.h>//get*uid, passwd
#include <sys/stat.h>//stat, S_*

#include <libconfig.h>//config_*

#include "util.h"
#include "adom.h"
#include "config.h"
#include "problem.h"
#include "log.h"
#include "put.h"
#include "def.h"

FILE * error_stream;
FILE * warning_stream;
FILE * note_stream;
FILE * call_stream;

/**
Runs the executable.

@param argc The amount of command line arguments.
@param argv The command line arguments.
@return The return code of the executable if successful and <code>EXECUTION_PROBLEM</code> otherwise.
**/
int main(int argc, char ** argv) {
	struct stat buf;

	/*
	Updates the count file of the executable.
	*/
	if (executable_count_file != NULL) {
		FILE * stream = fopen(executable_count_path, "wb");
		if (stream == NULL) {
			error(COUNT_OPEN_PROBLEM);
		}
		else {
			if (fwrite(generations, 1, sizeof generations, stream) != sizeof generations) {
				error(COUNT_WRITE_PROBLEM);
			}
			if (fclose(stream) != 0) {
				error(COUNT_CLOSE_PROBLEM);
			}
		}
	}

	/*
	Verifies the version of the executable.
	*/
	if (executable_version_file != NULL) {
		FILE * stream = fopen(executable_version_path, "rb");
		if (stream == NULL) {
			warning(VERSION_OPEN_PROBLEM);
		}
		else {
			unsigned char version[sizeof executable_version];
			if (fread(version, 1, sizeof version, stream) != sizeof version) {
				error(VERSION_READ_PROBLEM);
			}
			if (fclose(stream) != 0) {
				error(VERSION_CLOSE_PROBLEM);
			}
			if (memcmp(version, executable_version, sizeof version) != 0) {
				error(VERSION_MISMATCH_PROBLEM);
			}
		}
	}

	/*
	Removes the process file of the executable.
	*/
	if (executable_process_path != NULL) {
		if (unlink(executable_process_path) != 0) {
			return error(PROCESS_UNLINK_PROBLEM);
		}
	}

	/*
	Reports good news.
	*/
	printf("Loading...\n");
	sleep(1);

	/*
	Launches the executable.

	This process is replaced by the executable and all memory is automatically deallocated.
	*/
	argc--; argv++;//removes the first argument
	if (execvp(executable_path, argv) == 0) return NO_PROBLEM;//never returns

	/*
	Unloads the configuration file.

	The memory allocated by the <code>config_lookup_*</code> calls is automatically deallocated.
	*/
	config_destroy(&config);

	return error(EXEC_PROBLEM);
}

#endif
