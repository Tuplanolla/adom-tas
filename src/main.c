/**
Launches the executable.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef MAIN_C
#define MAIN_C

#include <stdlib.h>//*env, NULL, EXIT_*
#include <stdio.h>//*open, *close, *read, *write, FILE
#include <string.h>//str*, mem*
#include <unistd.h>//exec*, unlink
#include <sys/stat.h>//stat

#include "util.h"//intern
#include "problem.h"//*_PROBLEM
#include "log.h"//error, warning, note, PROPAGATE
#include "exec.h"//executable_*
#include "config.h"//*

/**
Prepares and launches the executable.

Command-line arguments are passed through.

@param argc The amount of command-line arguments.
@param argv The command-line arguments.
@return Nothing if successful and something else otherwise.
**/
int main(const int argc, char * const argv[]) {
	PROPAGATE(init_external_config());

	/*
	Enforces the default configuration.

	Similar to the command-line option -n.
	*/
	if (executable_config_path != NULL) {
		FILE * const stream = fopen(executable_config_path, "w");
		if (stream == NULL) {
			error(CONFIG_OPEN_PROBLEM);
		}
		else {
			const size_t size = strlen(executable_config) + 1;
			if (fwrite(&executable_config, size, 1, stream) != 1) {
				error(CONFIG_WRITE_PROBLEM);
			}
			if (fclose(stream) == EOF) {
				error(CONFIG_CLOSE_PROBLEM);
			}
		}
	}

	/*
	Removes the process lock.
	*/
	if (executable_process_path != NULL) {
		struct stat process_stat;
		if (stat(executable_process_path, &process_stat) == 0) {
			if (unlink(executable_process_path) != 0) {
				return error(PROCESS_UNLINK_PROBLEM);
			}
		}
	}

	/*
	Enforces the default keybindings.

	Similar to the command-line option -k.
	*/
	if (executable_keybind_path != NULL) {
		FILE * const stream = fopen(executable_keybind_path, "w");
		if (stream == NULL) {
			error(KEYBIND_OPEN_PROBLEM);
		}
		else {
			const size_t size = strlen(executable_keybind) + 1;
			if (fwrite(&executable_keybind, size, 1, stream) != 1) {
				error(KEYBIND_WRITE_PROBLEM);
			}
			if (fclose(stream) == EOF) {
				error(KEYBIND_CLOSE_PROBLEM);
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
			if (fclose(stream) == EOF) {
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
			if (fclose(stream) == EOF) {
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
	else if (setenv("LD_PRELOAD", loader_path, TRUE) == -1) {
		return error(LD_PRELOAD_SETENV_PROBLEM);
	}

	/*
	Launches the executable.
	*/
	if (executable_path == NULL) {
		return error(NULL_PROBLEM);
	}
	else if (execv(executable_path, argv) == -1) {
		return error(EXEC_PROBLEM);
	}

	PROPAGATE(uninit_config());

	/*
	Never returns.

	An <code>exec*</code> call either
		replaces this process with the executable or
		fails and returns the appropriate error code
			so this statement is never reached.
	*/
	return error(ASSERT_PROBLEM);
}

#endif
