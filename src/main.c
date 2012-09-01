/**
Launches the executable.

@file main.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#include <stdlib.h>//*env, NULL
#include <stdio.h>//*open, *close, *read, *write, FILE
#include <string.h>//str*, mem*
#include <unistd.h>//exec*, unlink
#include <sys/stat.h>//stat

#include "gnu.h"//__*__
#include "prob.h"//probno, *_PROBLEM
#include "def.h"//def_*
#include "cfg.h"//cfg_*
#include "log.h"//log_*
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
	if (cfg_init_main() == -1) {
		return probno;
	}

	/*
	Enforces the default configuration.

	Similar to the command-line option -n.
	*/
	if (cfg_exec_config_path != NULL) {
		FILE * const stream = fopen(cfg_exec_config_path, "w");
		if (stream == NULL) {
			log_error(CONFIG_OPEN_PROBLEM);
		}
		else {
			const size_t size = strlen(def_exec_config) + 1;
			if (fwrite(def_exec_config, size, 1, stream) != 1) {
				log_error(CONFIG_WRITE_PROBLEM);
			}
			if (fclose(stream) == EOF) {
				log_error(CONFIG_CLOSE_PROBLEM);
			}
		}
	}

	/*
	Removes the process lock.
	*/
	if (cfg_exec_process_path != NULL) {
		struct stat process_stat;
		if (stat(cfg_exec_process_path, &process_stat) == 0) {
			if (unlink(cfg_exec_process_path) != 0) {
				return log_error(PROCESS_UNLINK_PROBLEM);
			}
		}
	}

	/*
	Enforces the default keybindings.

	Similar to the command-line option -k.
	*/
	if (cfg_exec_keybind_path != NULL) {
		FILE * const stream = fopen(cfg_exec_keybind_path, "w");
		if (stream == NULL) {
			log_error(KEYBIND_OPEN_PROBLEM);
		}
		else {
			const size_t size = strlen(def_exec_keybind) + 1;
			if (fwrite(def_exec_keybind, size, 1, stream) != 1) {
				log_error(KEYBIND_WRITE_PROBLEM);
			}
			if (fclose(stream) == EOF) {
				log_error(KEYBIND_CLOSE_PROBLEM);
			}
		}
	}

	/*
	Verifies the version.
	*/
	if (cfg_exec_version_path != NULL) {
		FILE * const stream = fopen(cfg_exec_version_path, "rb");
		if (stream == NULL) {
			log_error(VERSION_OPEN_PROBLEM);
		}
		else {
			unsigned char version[sizeof exec_version];
			if (fread(version, sizeof version, 1, stream) != 1) {
				log_error(VERSION_READ_PROBLEM);
			}
			if (fclose(stream) == EOF) {
				log_error(VERSION_CLOSE_PROBLEM);
			}
			if (memcmp(version, exec_version, sizeof version) != 0) {
				log_error(VERSION_MISMATCH_PROBLEM);
			}
		}
	}

	/*
	Removes the error log.
	*/
	if (cfg_exec_error_path != NULL) {
		struct stat error_stat;
		if (stat(cfg_exec_error_path, &error_stat) == 0) {
			if (unlink(cfg_exec_error_path) != 0) {
				return log_error(ERROR_UNLINK_PROBLEM);
			}
		}
	}

	/*
	Sets the amount of generated characters.
	*/
	if (cfg_exec_count_path != NULL) {
		FILE * const stream = fopen(cfg_exec_count_path, "wb");
		if (stream == NULL) {
			log_error(COUNT_OPEN_PROBLEM);
		}
		else {
			if (fwrite(&cfg_generations, sizeof cfg_generations, 1, stream) != 1) {
				log_error(COUNT_WRITE_PROBLEM);
			}
			if (fclose(stream) == EOF) {
				log_error(COUNT_CLOSE_PROBLEM);
			}
		}
	}

	/*
	Enables preloading libraries.
	*/
	if (cfg_lib_path == NULL) {
		return log_error(NULL_PROBLEM);
	}
	else if (setenv("LD_PRELOAD", cfg_lib_path, TRUE) == -1) {
		return log_error(LD_PRELOAD_SETENV_PROBLEM);
	}

	/*
	Launches the executable.
	*/
	if (cfg_exec_path == NULL) {
		return log_error(NULL_PROBLEM);
	}
	else if (execv(cfg_exec_path, argv) == -1) {
		return log_error(EXEC_PROBLEM);
	}

	/*
	Suppresses warnings.

	An <code>exec*</code> call either
	 replaces this process with the executable or
	 fails and returns the appropriate error code
	  so this statement is never reached and
	  only serves to hide warnings:
	<pre>
	'cfg_exec_path' is never deallocated [-Wleak]
	</pre>
	*/
	if (cfg_uninit() == -1) {
		return probno;
	}

	return ASSERT_PROBLEM;
}
