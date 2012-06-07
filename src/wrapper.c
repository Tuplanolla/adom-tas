/**
Serves as wrapper for the executable.
**/
#ifndef WRAPPER_C
#define WRAPPER_C

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <curses.h>
#include <libconfig.h>

#include "util.h"
#include "problem.h"
#include "log.h"

FILE * error_log;
FILE * warning_log;
FILE * note_log;
FILE * call_log;
FILE * input;
FILE * output;

/**
Runs the executable.

@param argc The amount of command line arguments.
@param argv The command line arguments.
@return The return code of the executable if successful and <code>EXECUTION_PROBLEM</code> otherwise.
**/
int main(int argc, char ** argv) {
	struct stat buf;

	error_log = stderr;
	warning_log = stderr;
	note_log = stderr;
	call_log = NULL;
	output = NULL;
	input = NULL;

	const char * const config_path = "adom-tas.cfg";

	const char * const config_file = ".adom.cfg";
	const char * const version_file = ".adom.ver";
	const char * const keybind_file = ".adom.kbd";
	const char * const process_file = ".adom.prc";

	/*
	Loads the configuration file.
	*/
	config_t config;
	config_init(&config);
	if (config_read_file(&config, config_path) == 0) {
		config_destroy(&config);
		return error(CONFIG_PROBLEM);
	}

	/*
	Finds the home directory.

	The configuration file is first parsed,
	the environment variable is then read,
	a system call then is used and
	the existence of the directory is finally checked.
	*/
	const char * home_path;
	if (config_lookup_string(&config, "home", &home_path) == 0) {
		home_path = getenv("HOME");
		if (home_path == NULL) {
			struct passwd * pw = getpwuid(getuid());
			if (pw == NULL) {
				return error(HOME_FIND_PROBLEM);
			}
			home_path = pw->pw_dir;
		}
	}
	if (stat(home_path, &buf) != 0) {
		return error(HOME_ACCESS_PROBLEM);
	}

	/*
	Enables loading libraries.

	The configuration file is first parsed,
	the environment variable is then read and
	the existence of the file is then checked.
	*/
	const char * libc_path;
	if (config_lookup_string(&config, "libc", &libc_path) == 0) {
		libc_path = getenv("LIBC");
		if (libc_path == NULL) {
			return error(LIBC_FIND_PROBLEM);
		}
	}
	if (stat(libc_path, &buf) != 0) {
		return error(LIBC_ACCESS_PROBLEM);
	}
	const char * libncurses_path;
	if (config_lookup_string(&config, "libncurses", &libncurses_path) == 0) {
		libncurses_path = getenv("LIBNCURSES");
		if (libncurses_path == NULL) {
			return error(LIBNCURSES_FIND_PROBLEM);
		}
	}
	if (stat(libncurses_path, &buf) != 0) {
		return error(LIBNCURSES_ACCESS_PROBLEM);
	}

	/*
	Enables preloading libraries.

	The configuration file is first parsed,
	the environment variable is then read,
	the existence of the file is then checked and
	the environment variable is finally set.
	*/
	const char * ld_preload_path;
	if (config_lookup_string(&config, "ld_preload", &ld_preload_path) == 0) {
		ld_preload_path = getenv("LD_PRELOAD");
		if (ld_preload_path == NULL) {
			return error(LD_PRELOAD_FIND_PROBLEM);
		}
	}
	if (stat(home_path, &buf) != 0) {
		return error(LD_PRELOAD_ACCESS_PROBLEM);
	}
	if (setenv("LD_PRELOAD", ld_preload_path, TRUE) != 0) {
		return error(LD_PRELOAD_SET_PROBLEM);
	}

	/*
	Finds the executable and identifies it heuristically.

	The configuration file is first parsed,
	the existence of the file is then checked,
	the permissions of the file are then checked,
	the size of the file is finally verified.
	*/
	const char * exec_path;
	if (config_lookup_string(&config, "exec", &exec_path) == 0) {
		return error(CONFIG_EXEC_PROBLEM);
	}
	if (stat(exec_path, &buf) != 0) {
		return error(EXEC_FIND_PROBLEM);
	}
	if (buf.st_mode & (S_ISUID | S_ISGID)) {
		return error(EXEC_ACCESS_PROBLEM);
	}
	if (buf.st_size != 2452608) {
		return error(EXEC_SIZE_PROBLEM);
	}

	/*
	Finds the data path of the executable.
	*/
	const char * data_path;
	if (config_lookup_string(&config, "data", &data_path) == 0) {
		return error(CONFIG_DATA_PROBLEM);
	}

	/*
	Verifies the version of the executable.
	*/
	{
		size_t size = strlen(data_path)+1+strlen(version_file)+1;
		char * version_path = malloc(size);
		snprintf(version_path, size, "%s/%s", data_path, version_file);
		FILE * stream = fopen(version_path, "r");
		if (stream == NULL) {
			warning(VERSION_WARNING);
		}
		else {
			unsigned char version[4];
			fread(version, 1, sizeof (version), stream);
			fclose(stream);
			unsigned char desired_version[4];
			desired_version[0] = 1;
			desired_version[1] = 1;
			desired_version[2] = 1;
			desired_version[3] = 0;
			if (memcmp(version, desired_version, sizeof (version)) != 0) {
				return error(VERSION_PROBLEM);
			}
		}
		free(version_path);
	}

	/*
	Removes the process file of the executable.
	*/
	{
		size_t size = strlen(data_path)+1+strlen(process_file)+1;
		char * process_path = malloc(size);
		snprintf(process_path, size, "%s/%s", data_path, process_file);
		if (stat(process_path, &buf) == 0) {
			if (unlink(process_path) == 0) {
				return error(PROCESS_PROBLEM);
			}
		}
		free(process_path);
	}

	/*
	Opens the key streams.
	*/
	const char * input_path;
	if (config_lookup_string(&config, "input", &input_path) == 0) {
		warning(CONFIG_INPUT_PROBLEM);
	}
	else {
		if (stat(input_path, &buf) == 0) {
			warning(INPUT_FIND_PROBLEM);
		}
		/*
		input = fopen(input_path, "rb");//opened later
		if (input == NULL) {
			error(INPUT_ACCESS_PROBLEM);
		}
		*/
	}
	const char * output_path;
	if (config_lookup_string(&config, "output", &output_path) == 0) {
		return error(CONFIG_OUTPUT_PROBLEM);
	}
	else {
		if (stat(output_path, &buf) == 0) {
			warning(OUTPUT_OVERWRITE_PROBLEM);
		}
		/*
		output = fopen(output_path, "wb");//opened later
		if (output == NULL) {
			error(OUTPUT_ACCESS_PROBLEM);
		}
		*/
	}

	/*
	Opens the log streams.
	*/
	const char * error_log_path;
	if (config_lookup_string(&config, "error_log", &error_log_path) == 0) {
		error_log = stderr;
		warning(CONFIG_ERROR_LOG_PROBLEM);
	}
	else {
		if (stat(error_log_path, &buf) == 0) {
			note(ERROR_LOG_OVERWRITE_PROBLEM);
		}
		error_log = fopen(error_log_path, "w");
		if (error_log == NULL) {
			error_log = stderr;
			warning(ERROR_LOG_ACCESS_PROBLEM);
		}
	}
	const char * warning_log_path;
	if (config_lookup_string(&config, "warning_log", &warning_log_path) == 0) {
		warning_log = stderr;
		warning(CONFIG_WARNING_LOG_PROBLEM);
	}
	else {
		if (stat(warning_log_path, &buf) == 0) {
			note(WARNING_LOG_OVERWRITE_PROBLEM);
		}
		warning_log = fopen(warning_log_path, "w");
		if (warning_log == NULL) {
			warning_log = stderr;
			warning(WARNING_LOG_ACCESS_PROBLEM);
		}
	}
	const char * note_log_path;
	if (config_lookup_string(&config, "note_log", &note_log_path) == 0) {
		note_log = stderr;
		warning(CONFIG_NOTE_LOG_PROBLEM);
	}
	else {
		if (stat(note_log_path, &buf) == 0) {
			note(NOTE_LOG_OVERWRITE_PROBLEM);
		}
		note_log = fopen(note_log_path, "w");
		if (note_log == NULL) {
			note_log = stderr;
			warning(NOTE_LOG_ACCESS_PROBLEM);
		}
	}
	const char * call_log_path;
	if (config_lookup_string(&config, "call_log", &call_log_path) == 0) {
		warning(CONFIG_CALL_LOG_PROBLEM);
	}
	else {
		if (stat(call_log_path, &buf) == 0) {
			note(CALL_LOG_OVERWRITE_PROBLEM);
		}
		call_log = fopen(call_log_path, "w");
		if (call_log == NULL) {
			warning(CALL_LOG_ACCESS_PROBLEM);
		}
		else {
			note(CALL_LOG_PROBLEM);
		}
	}

	/*
	Unloads the configuration file.

	The memory allocated by <code>config_lookup_string</code> calls is automatically deallocated.
	*/
	config_destroy(&config);

	/*
	Launches the executable.

	This process is replaced with new process.
	*/
	argc--; argv++;//removes the first argument
	if (execvp(exec_path, argv) == 0) return NO_PROBLEM;//never returns
	return error(EXECUTION_PROBLEM);
}

#endif
