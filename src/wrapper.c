/**
Launches the executable.
**/
#ifndef WRAPPER_C
#define WRAPPER_C

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <curses.h>
#include <libconfig.h>

#include "util.h"
#include "problem.h"
#include "log.h"
#include "put.h"
#include "config.h"

char * exec_file;
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
	STDSTR(error_stream, default_error_name);
	STDSTR(warning_stream, default_warning_name);
	STDSTR(note_stream, default_note_name);
	STDSTR(call_stream, default_call_name);

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
		fprintf(stream, default_config_str);
		fclose(stream);
		return error(CONFIG_FIND_PROBLEM);
	}
	config_t config;
	config_init(&config);
	if (config_read_file(&config, default_config_name) == 0) {
		config_destroy(&config);
		return error(CONFIG_PROBLEM);
	}

	/*
	Enables preloading libraries.

	The configuration file is first searched,
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
	if (stat(ld_preload_path, &buf) != 0) {
		return error(LD_PRELOAD_ACCESS_PROBLEM);
	}
	if (setenv("LD_PRELOAD", ld_preload_path, TRUE) != 0) {
		return error(LD_PRELOAD_SET_PROBLEM);
	}

	/*
	Finds the executable and identifies it heuristically.

	The configuration file is first searched,
	the existence of the file is then checked,
	the type of the file is then checked,
	the special permissions of the file are then checked and
	the size of the file is finally verified.
	*/
	const char * exec_path;
	if (config_lookup_string(&config, "exec", &exec_path) == 0) {
		return error(CONFIG_EXEC_PROBLEM);
	}
	if (stat(exec_path, &buf) != 0) {
		return error(EXEC_FIND_PROBLEM);
	}
	if (S_ISDIR(buf.st_mode)) {
		return error(EXEC_TYPE_PROBLEM);
	}
	if ((buf.st_mode & (S_ISUID | S_ISGID)) != 0) {
		return error(EXEC_ACCESS_PROBLEM);
	}
	if (buf.st_size != file_size) {
		return error(EXEC_SIZE_PROBLEM);
	}
	exec_file = malloc(strlen(exec_path)+1);
	strcpy(exec_file, exec_path);

	/*
	Finds the data path of the executable.

	The configuration file is first searched and
	the existence of the directory is then checked.
	*/
	const char * data_path;
	if (config_lookup_string(&config, "data", &data_path) == 0) {
		return error(CONFIG_DATA_PROBLEM);
	}
	if (stat(data_path, &buf) != 0) {
		return error(DATA_FIND_PROBLEM);
	}

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
		FILE * stream = fopen(version_path, "r");
		if (stream == NULL) {
			warning(VERSION_FIND_PROBLEM);
		}
		else {
			unsigned char version[sizeof (file_version)];
			fread(version, 1, sizeof (version), stream);
			fclose(stream);
			if (memcmp(version, file_version, sizeof (version)) != 0) {
				return error(VERSION_PROBLEM);
			}
		}
		free(version_path);
	}

	/*
	Removes the process file of the executable.

	The configuration file is first searched,
	the existence of the process file is then checked and
	the process file is finally removed.
	*/
	{
		const size_t size = strlen(data_path)+1+strlen(process_file)+1;
		char * process_path = malloc(size);
		snprintf(process_path, size, "%s/%s", data_path, process_file);
		if (stat(process_path, &buf) == 0) {
			if (unlink(process_path) != 0) {
				return error(PROCESS_PROBLEM);
			}
		}
		free(process_path);
	}

	/*
	Unloads the configuration file.

	The memory allocated by the <code>config_lookup_</code> calls is automatically deallocated.
	However the memory allocated for <code>exec_file</code> is leaked.
	*/
	config_destroy(&config);

	/*
	Reports good news.
	*/
	printf("Loading...\n");
	sleep(1);

	/*
	Launches the executable.

	This process is replaced by the executable.
	*/
	argc--; argv++;//removes the first argument
	if (execvp(exec_file, argv) == 0) return NO_PROBLEM;//never returns
	return error(EXEC_PROBLEM);
}

#endif
