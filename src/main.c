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

#include "gnu.h"//__*__
#include "util.h"//intern
#include "prob.h"//PROPAGATE*, *_PROBLEM
#include "log.h"//error, warning, note
#include "def.h"//def_*
#include "cfg.h"//*
#include "exec.h"//exec_*

/**
Prepares and launches the executable.

Command-line arguments are passed through.

@param argc The amount of command-line arguments.
@param argv The command-line arguments.
@return Nothing if successful and something else otherwise.
**/
int main(const int argc __attribute__ ((unused)), char * const argv[]) {
	/*
	Initializes the configuration variables.
	*/
	PROPAGATE(init_main_config());

	/*
	Enforces the default configuration.

	Similar to the command-line option -n.
	*/
	if (exec_config_path != NULL) {
		FILE * const stream = fopen(exec_config_path, "w");
		if (stream == NULL) {
			error(CONFIG_OPEN_PROBLEM);
		}
		else {
			const size_t size = strlen(def_exec_config) + 1;
			if (fwrite(def_exec_config, size, 1, stream) != 1) {
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
	if (exec_process_path != NULL) {
		struct stat process_stat;
		if (stat(exec_process_path, &process_stat) == 0) {
			if (unlink(exec_process_path) != 0) {
				return error(PROCESS_UNLINK_PROBLEM);
			}
		}
	}

	/*
	Enforces the default keybindings.

	Similar to the command-line option -k.
	*/
	if (exec_keybind_path != NULL) {
		FILE * const stream = fopen(exec_keybind_path, "w");
		if (stream == NULL) {
			error(KEYBIND_OPEN_PROBLEM);
		}
		else {
			const size_t size = strlen(def_exec_keybind) + 1;
			if (fwrite(def_exec_keybind, size, 1, stream) != 1) {
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
	if (exec_version_path != NULL) {
		FILE * const stream = fopen(exec_version_path, "rb");
		if (stream == NULL) {
			error(VERSION_OPEN_PROBLEM);
		}
		else {
			unsigned char version[sizeof exec_version];
			if (fread(version, sizeof version, 1, stream) != 1) {
				error(VERSION_READ_PROBLEM);
			}
			if (fclose(stream) == EOF) {
				error(VERSION_CLOSE_PROBLEM);
			}
			if (memcmp(version, exec_version, sizeof version) != 0) {
				error(VERSION_MISMATCH_PROBLEM);
			}
		}
	}

	/*
	Removes the error log.
	*/
	if (exec_error_path != NULL) {
		struct stat error_stat;
		if (stat(exec_error_path, &error_stat) == 0) {
			if (unlink(exec_error_path) != 0) {
				return error(ERROR_UNLINK_PROBLEM);
			}
		}
	}

	/*
	Sets the amount of generated characters.
	*/
	if (exec_count_path != NULL) {
		FILE * const stream = fopen(exec_count_path, "wb");
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
	if (lib_path == NULL) {
		return error(NULL_PROBLEM);
	}
	else if (setenv("LD_PRELOAD", lib_path, TRUE) == -1) {
		return error(LD_PRELOAD_SETENV_PROBLEM);
	}

	/*
	Launches the executable.
	*/
	if (exec_path == NULL) {
		return error(NULL_PROBLEM);
	}
	else if (execv(exec_path, argv) == -1) {
		return error(EXEC_PROBLEM);
	}

	/*
	Suppresses warnings:
	<pre>
	'exec_path' is never deallocated [-Wleak]
	</pre>
	*/
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
