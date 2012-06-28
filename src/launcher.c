/**
Launches the executable.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef LAUNCHER_C
#define LAUNCHER_C

#include <stdlib.h>//NULL
#include <stdio.h>//*open, *read, *write, *close, *print*, std*, FILE
#include <string.h>//mem*, unlink
#include <unistd.h>//exec*

#include "util.h"//intern
#include "problem.h"//*_PROBLEM
#include "log.h"//error, warning, note
#include "exec.h"//executable_*
#include "config.h"//*_config, executable_*, generations

intern char * executable_path;
intern char * executable_config_path;
intern char * executable_process_path;
intern char * executable_keybind_path;
intern char * executable_version_path;
intern char * executable_count_path;
intern char * loader_path;
intern unsigned int generations;
intern const unsigned char executable_version[4];

/**
Runs the executable.

The command-line arguments are passed through.

@param argc The amount of command-line arguments.
@param argv The command-line arguments.
@return <code>NO_PROBLEM</code> if successful and something else otherwise.
**/
int main(const int argc, char * argv[]) {
	/*
	Loads the configuration.
	*/
	PROPAGATE(init_launcher_config());

	/*
	Removes the process file.
	*/
	if (executable_process_path != NULL) {
		if (unlink(executable_process_path) != 0) {
			return error(PROCESS_UNLINK_PROBLEM);
		}
	}

	/*
	Verifies the version.
	*/
	if (executable_version_path != NULL) {
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
	Sets the amount of generated characters.
	*/
	if (executable_count_path != NULL) {
		FILE * stream = fopen(executable_count_path, "wb");
		if (stream == NULL) {
			error(COUNT_OPEN_PROBLEM);
		}
		else {
			if (fwrite(&generations, sizeof generations, 1, stream) != 1) {
				error(COUNT_WRITE_PROBLEM);
			}
			if (fclose(stream) != 0) {
				error(COUNT_CLOSE_PROBLEM);
			}
		}
	}

	/*
	Enables preloading libraries.
	*/
	if (setenv("LD_PRELOAD", loader_path, TRUE) != 0) {
		return error(LD_PRELOAD_SETENV_PROBLEM);
	}

	/*
	Launches the executable.
	*/
	if (executable_path != NULL) {
		argv[0] = executable_path;
		if (execv(executable_path, argv) != 0) {
			return error(EXEC_PROBLEM);
		}
	}

	/*
	Suppresses warnings.

	An <code>exec*</code> call either
		replaces this process with the executable or
		fails and returns the appropriate error code.
	*/
	return error(CAUSALITY_PROBLEM);
}

#endif
