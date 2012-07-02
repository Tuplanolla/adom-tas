/**
Launches the executable.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef MAIN_C
#define MAIN_C

#include <stdlib.h>//*env, NULL
#include <stdio.h>//*open, *close, *read, *write, FILE
#include <string.h>//str*, mem*
#include <unistd.h>//exec*, unlink
#include <sys/stat.h>//stat

#include "util.h"//intern
#include "problem.h"//*_PROBLEM
#include "log.h"//error, warning, note, PROPAGATE
#include "exec.h"//executable_*
#include "config.h"//*_config, executable_*, generations

/**
The configuration variables.
**/
intern char * executable_path;
intern char * executable_temporary_path;
intern char ** executable_temporary_paths;
intern char * executable_config_path;
intern char * executable_process_path;
intern char * executable_keybind_path;
intern char * executable_version_path;
intern char * executable_count_path;
intern char * loader_path;
intern unsigned int generations;

/**
The variables of the executable.
**/
intern const unsigned char executable_version[4];
intern const unsigned int executable_temporary_levels;
intern const unsigned int executable_temporary_parts;

/**
Prepares and launches the executable.

The command-line arguments are passed through.

@param argc The amount of command-line arguments.
@param argv The command-line arguments.
@return <code>NO_PROBLEM</code> if successful and something else otherwise.
**/
int main(const int argc, char * const argv[]) {
	/*
	Loads the configuration.
	*/
	PROPAGATE(init_launcher_config());
	/*if (code != NO_PROBLEM) {
		uninit_config();
		return code;
	}*/

	/*
	Removes the temporary files.
	*/
	if (executable_temporary_paths != NULL) {
		for (unsigned int level = 0; level < executable_temporary_levels; level++) {
			const unsigned int offset = level * executable_temporary_parts;
			for (unsigned int part = 0; part < executable_temporary_parts; part++) {
				const unsigned int path = offset + part;
				if (executable_temporary_paths[path] != NULL) {
					struct stat buf;
					if (stat(executable_temporary_paths[path], &buf) == 0) {
						if (unlink(executable_temporary_paths[path]) != 0) {
							return error(TEMPORARY_UNLINK_PROBLEM);
						}
					}
				}
			}
		}
	}

	/*
	Removes the process lock.
	*/
	if (executable_process_path != NULL) {
		struct stat buf;
		if (stat(executable_process_path, &buf) == 0) {
			if (unlink(executable_process_path) != 0) {
				return error(PROCESS_UNLINK_PROBLEM);
			}
		}
	}

	/*
	Verifies the version.
	*/
	if (executable_version_path != NULL) {
		FILE * const stream = fopen(executable_version_path, "rb");
		if (stream == NULL) {
			error(VERSION_OPEN_PROBLEM);
		}
		else {
			unsigned char version[sizeof executable_version];
			if (fread(version, sizeof version, 1, stream) != 1) {
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
		FILE * const stream = fopen(executable_count_path, "wb");
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
	if (loader_path == NULL) {
		return error(NULL_PROBLEM);
	}
	else {
		if (setenv("LD_PRELOAD", loader_path, TRUE) == -1) {
			return error(LD_PRELOAD_SETENV_PROBLEM);
		}
	}

	/*
	Launches the executable.
	*/
	if (executable_path == NULL) {
		return error(NULL_PROBLEM);
	}
	else {
		if (execv(executable_path, argv) == -1) {
			return error(EXEC_PROBLEM);
		}
	}

	/*
	Never returns.

	An <code>exec*</code> call either
		replaces this process with the executable or
		fails and returns the appropriate error code
			so this statement is never reached.
	*/
	return error(NULL_PROBLEM);
}

#endif