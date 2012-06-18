/**
Launches the executable.

@see CONFIG_OPEN_PROBLEM
@see CONFIG_WRITE_PROBLEM
@see CONFIG_CLOSE_PROBLEM
@see CONFIG_STAT_PROBLEM
@see CONFIG_PARSE_PROBLEM
@see HOME_CONFIG_PROBLEM
@see HOME_GETENV_PROBLEM
@see HOME_GETPWUID_PROBLEM
@see HOME_GUESS_PROBLEM
@see HOME_STAT_PROBLEM
@see LD_PRELOAD_CONFIG_PROBLEM
@see LD_PRELOAD_GETENV_PROBLEM
@see LD_PRELOAD_GUESS_PROBLEM
@see LD_PRELOAD_STAT_PROBLEM
@see LD_PRELOAD_SETENV_PROBLEM
@see EXECUTABLE_CONFIG_PROBLEM
@see EXECUTABLE_STAT_PROBLEM
@see EXECUTABLE_TYPE_PROBLEM
@see EXECUTABLE_PERMISSION_PROBLEM
@see EXECUTABLE_SIZE_PROBLEM
@see EXECUTABLE_OPEN_PROBLEM
@see EXECUTABLE_READ_PROBLEM
@see EXECUTABLE_HASH_PROBLEM
@see DATA_CONFIG_PROBLEM
@see DATA_STAT_PROBLEM
@see VERSION_OPEN_PROBLEM
@see VERSION_MISMATCH_PROBLEM
@see PROCESS_STAT_PROBLEM
@see PROCESS_UNLINK_PROBLEM
@see EXEC_PROBLEM
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef LAUNCHER_C
#define LAUNCHER_C

#include <stdlib.h>//*alloc, free, *env
#include <stdio.h>//*print*, *open, *close, std*, FILE
#include <string.h>//str*
#include <sys/stat.h>//stat, S_*

#include <libconfig.h>//config_*, config_t

#include "util.h"
#include "problem.h"
#include "log.h"
#include "put.h"
#include "config.h"

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
	Sets the streams to their default values.
	*/
	error_stream = stdstr(default_error_name);
	warning_stream = stdstr(default_warning_name);
	note_stream = stdstr(default_note_name);
	call_stream = stdstr(default_call_name);

	/*
	Prepares the configuration file.

	The existence of the configuration file is first checked,
	if it does not exist
		the configuration file is first created and
		this process is then terminated,
	otherwise
		the configuration file is parsed.
	*/
	if (stat(default_config_name, &buf) != 0) {
		FILE * stream = fopen(default_config_name, "w");
		if (stream == NULL) {
			return error(CONFIG_OPEN_PROBLEM);
		}
		else {
			if (fprintf(stream, default_config_str) != strlen(default_config_str)) {
				error(CONFIG_WRITE_PROBLEM);
			}
			if (fclose(stream) != 0) {
				return error(CONFIG_CLOSE_PROBLEM);
			}
		}
		return note(CONFIG_STAT_PROBLEM);
	}
	config_t config;
	config_init(&config);
	if (config_read_file(&config, default_config_name) == 0) {
		config_destroy(&config);
		return error(CONFIG_PARSE_PROBLEM);
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
		warning(HOME_CONFIG_PROBLEM);
		home_path = getenv("HOME");
		if (home_path == NULL) {
			warning(HOME_GETENV_PROBLEM);
			struct passwd * pw = getpwuid(getuid());
			if (pw == NULL) {
				warning(HOME_GETPWUID_PROBLEM);
				note(HOME_GUESS_PROBLEM);
				home_path = default_home_name;
			}
			else {
				home_path = pw->pw_dir;
			}
		}
	}
	if (stat(home_path, &buf) != 0) {
		warning(HOME_STAT_PROBLEM);
		//now shell stuff is impossible
	}

	/*
	Enables preloading libraries.

	The configuration file is first searched,
	the environment variable is then gotten,
	the existence of the file is then checked and
	the environment variable is finally set.
	*/
	const char * ld_preload_path;
	if (config_lookup_string(&config, "loader", &ld_preload_path) == 0) {
		warning(LD_PRELOAD_CONFIG_PROBLEM);
		ld_preload_path = getenv("LD_PRELOAD");
		if (ld_preload_path == NULL) {
			warning(LD_PRELOAD_GETENV_PROBLEM);
			note(LD_PRELOAD_GUESS_PROBLEM);
			ld_preload_path = default_ld_preload_name;
		}
	}
	if (stat(ld_preload_path, &buf) != 0) {
		return error(LD_PRELOAD_STAT_PROBLEM);
	}
	else if (setenv("LD_PRELOAD", ld_preload_path, TRUE) != 0) {
		return error(LD_PRELOAD_SETENV_PROBLEM);
	}

	/*
	Finds the executable and identifies it heuristically.

	The configuration file is first searched,
	the existence of the file is then checked,
	the type of the file is then checked,
	the special permissions of the file are then checked,
	the size of the file is then verified and
	the hash code of the file is finally verified.
	*/
	const char * exec_path;
	if (config_lookup_string(&config, "executable", &exec_path) == 0) {
		return error(EXECUTABLE_CONFIG_PROBLEM);
	}
	else if (stat(exec_path, &buf) != 0) {
		return error(EXECUTABLE_STAT_PROBLEM);
	}
	else if (S_ISDIR(buf.st_mode)) {
		return error(EXECUTABLE_TYPE_PROBLEM);
	}
	else if ((buf.st_mode & (S_ISUID | S_ISGID)) != 0) {
		return error(EXECUTABLE_PERMISSION_PROBLEM);
	}
	else {
		const size_t exec_size = buf.st_size;
		if (exec_size != file_size) {
			warning(EXECUTABLE_SIZE_PROBLEM);
		}
		FILE * stream = fopen(exec_path, "rb");
		if (stream == NULL) {
			warning(EXECUTABLE_OPEN_PROBLEM);
		}
		else {
			unsigned char * ptr = malloc(exec_size);
			const size_t size = fread(ptr, 1, exec_size, stream);
			if (size != exec_size) {
				warning(EXECUTABLE_READ_PROBLEM);
			}
			else if (hash(ptr, exec_size) != file_hash) {
				warning(EXECUTABLE_HASH_PROBLEM);
			}
			free(ptr);
			fclose(stream);
		}
	}

	/*
	Finds the data path of the executable.

	The configuration file is first searched and
	the existence of the directory is then checked.
	*/
	const char * data_path;
	if (config_lookup_string(&config, "data", &data_path) == 0) {
		warning(DATA_CONFIG_PROBLEM);
	}
	else {
		//guess ~/.adom.data
	}
	if (stat(data_path, &buf) != 0) {
		return error(DATA_STAT_PROBLEM);
	}

	/*
	Updates the count file of the executable.

	The existence of the count file is first checked and
	the count file is then replaced?
	*/

	/*
	Verifies the version of the executable.

	The configuration file is first searched,
	the version file is then read and
	the version is finally verified.
	*/
	{
		const size_t size = strlen(data_path)+1+strlen(version_file)+1;
		char * version_path = malloc(size);
		snprintf(version_path, size, "%s/%s", data_path, version_file);
		//stat first
		FILE * stream = fopen(version_path, "r");
		if (stream == NULL) {
			warning(VERSION_OPEN_PROBLEM);
		}
		else {
			unsigned char version[sizeof (file_version)];
			fread(version, 1, sizeof (version), stream);
			fclose(stream);
			if (memcmp(version, file_version, sizeof (file_version)) != 0) {
				return error(VERSION_MISMATCH_PROBLEM);
			}
		}
		free(version_path);
	}

	/*
	Removes the process file of the executable.

	The existence of the process file is first checked and
	the process file is then removed.
	*/
	{
		const size_t size = strlen(data_path)+1+strlen(process_file)+1;
		char * process_path = malloc(size);
		snprintf(process_path, size, "%s/%s", data_path, process_file);
		if (stat(process_path, &buf) == 0) {
			note(PROCESS_STAT_PROBLEM);
			if (unlink(process_path) != 0) {
				return error(PROCESS_UNLINK_PROBLEM);
			}
		}
		free(process_path);
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
	if (execvp(exec_file, argv) == 0) return NO_PROBLEM;//never returns

	/*
	Unloads the configuration file.

	The memory allocated by the <code>config_lookup_*</code> calls is automatically deallocated.
	*/
	config_destroy(&config);

	return error(EXEC_PROBLEM);
}

#endif
