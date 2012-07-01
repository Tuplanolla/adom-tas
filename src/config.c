/**
Manages the configuration variables.

Some redundant conditions are used to
	enforce predictable behavior and
	make maintenance (return point changes) easier.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef CONFIG_C
#define CONFIG_C

#include <stdlib.h>//*alloc, free, *env, size_t, NULL
#include <stdio.h>//*open, *close, *read, *write, *print*, FILE, EOF
#include <string.h>//str*
#include <sys/stat.h>//stat, S_*
#include <pwd.h>//*id, *wd

#include <libconfig.h>//config_*

#include "util.h"//intern, hash, stdstr, SUBNULL
#include "problem.h"//problem_t, *_PROBLEM
#include "log.h"//error, warning, note
#include "def.h"//default_*
#include "exec.h"//executable_*

#include "config.h"

/**
The default configuration variables.
**/
intern const char * const default_executable_path;
intern const char * const default_loader_path;
intern const char * const default_libc_path;
intern const char * const default_libncurses_path;
intern const int default_generations;
intern const int default_states;
intern const int default_rows;
intern const int default_cols;
intern const char * const default_iterator;
intern const char * const default_input_path;
intern const char * const default_output_path;
intern const char * const default_shm_path;
intern const char * const default_error_stream;
intern const char * const default_warning_stream;
intern const char * const default_note_stream;
intern const char * const default_call_stream;
intern const bool default_autoplay;
intern const char * const default_config_path;
intern const char * const default_config;

/**
The variables of the executable.
**/
intern const size_t executable_size;
intern const int executable_hash;
intern const unsigned char executable_version[4];
intern const int executable_rows_min;
intern const int executable_cols_min;
intern const int executable_rows_max;
intern const int executable_cols_max;
intern const char * const executable_data_file;
intern const char * const executable_temporary_file;
intern const char * const executable_config_file;
intern const char * const executable_process_file;
intern const char * const executable_keybind_file;
intern const char * const executable_version_file;
intern const char * const executable_count_file;

/**
The configuration variables.
**/
intern char * home_path;
intern char * executable_path;
intern char * executable_data_path;
intern char * executable_temporary_path;
intern char * executable_config_path;
intern char * executable_process_path;
intern char * executable_keybind_path;
intern char * executable_version_path;
intern char * executable_count_path;
intern char * loader_path;
intern char * libc_path;
intern char * libncurses_path;
intern unsigned int generations;
intern unsigned int states;
intern unsigned int rows;
intern unsigned int cols;
intern char * iterator;
intern FILE * input_stream;
intern FILE ** output_streams;
intern char * shm_path;
intern FILE * error_stream;
intern FILE * warning_stream;
intern FILE * note_stream;
intern FILE * call_stream;
intern bool autoplay;

/**
The configuration.
**/
config_t config;

/**
A buffer for <code>stat</code> calls.
**/
struct stat buf;

/**
A buffer for <code>getpwuid</code> calls.
**/
struct passwd * pw;

/**
Uninitializes the configuration variables.

@return The error code.
**/
problem_t uninit_config(void) {
	/*
	Closes files and deallocates manually allocated memory.
	*/
	if (home_path != NULL) {
		free(home_path);
	}
	if (executable_path != NULL) {
		free(executable_path);
	}
	if (executable_data_path != NULL) {
		free(executable_data_path);
	}
	if (executable_config_path != NULL) {
		free(executable_config_path);
	}
	if (executable_process_path != NULL) {
		free(executable_process_path);
	}
	if (executable_keybind_path != NULL) {
		free(executable_keybind_path);
	}
	if (executable_version_path != NULL) {
		free(executable_version_path);
	}
	if (executable_count_path != NULL) {
		free(executable_count_path);
	}
	if (loader_path != NULL) {
		free(loader_path);
	}
	if (libc_path != NULL) {
		free(libc_path);
	}
	if (libncurses_path != NULL) {
		free(libncurses_path);
	}
	if (iterator != NULL) {
		free(iterator);
	}
	if (input_stream != NULL) {
		if (fclose(input_stream) == EOF) {
			error(INPUT_CLOSE_PROBLEM);
		}
	}
	for (unsigned int state = 0; state < states; state++) {
		if (output_streams[state] != NULL) {
			if (fclose(output_streams[state]) == EOF) {
				error(OUTPUT_CLOSE_PROBLEM);
			}
		}
	}
	if (output_streams != NULL) {
		free(output_streams);
	}
	if (shm_path != NULL) {
		free(shm_path);
	}
	if (error_stream != NULL) {
		if (fclose(error_stream) == EOF) {
			error(ERROR_CLOSE_PROBLEM);
		}
	}
	if (warning_stream != NULL) {
		if (fclose(warning_stream) == EOF) {
			error(WARNING_CLOSE_PROBLEM);
		}
	}
	if (note_stream != NULL) {
		if (fclose(note_stream) == EOF) {
			error(NOTE_CLOSE_PROBLEM);
		}
	}
	if (call_stream != NULL) {
		if (fclose(call_stream) == EOF) {
			error(CALL_CLOSE_PROBLEM);
		}
	}

	return NO_PROBLEM;
}

/**
Initializes the common configuration variables.

@return The error code.
**/
problem_t init_config(void) {
	/*
	Sets the streams to their default values.
	*/
	error_stream = stdstr(default_error_stream);
	warning_stream = stdstr(default_warning_stream);
	note_stream = stdstr(default_note_stream);
	call_stream = stdstr(default_call_stream);

	/*
	Prepares the configuration file.

	The existence of the configuration file is first checked,
	if it does not exist
		the configuration file is first created and
		this process is then terminated,
	otherwise
		the configuration file is parsed.
	*/
	if (stat(default_config_path, &buf) == -1) {
		FILE * stream = fopen(default_config_path, "w");
		if (stream == NULL) {
			return error(CONFIG_OPEN_PROBLEM);
		}
		else {
			if ((size_t )fprintf(stream, "%s", default_config) != strlen(default_config)) {
				error(CONFIG_WRITE_PROBLEM);
			}
			if (fclose(stream) == EOF) {
				error(CONFIG_CLOSE_PROBLEM);
			}
		}
		return note(CONFIG_STAT_PROBLEM);
	}
	config_init(&config);
	if (config_read_file(&config, default_config_path) == 0) {
		return error(CONFIG_PARSE_PROBLEM);
	}

	/*
	Finds the home directory.

	The configuration file is first parsed,
	the environment variable is then read,
	a system call then is used and
	the existence of the directory is finally checked.
	*/
	const char * new_home_path;
	if (config_lookup_string(&config, "home", &new_home_path) == 0) {
		//warning(HOME_CONFIG_PROBLEM);
		new_home_path = getenv("HOME");
		if (new_home_path == NULL) {
			warning(HOME_GETENV_PROBLEM);
			pw = getpwuid(getuid());
			if (pw == NULL) {
				new_home_path = NULL;
				warning(HOME_GETPWUID_PROBLEM);
			}
			else {
				new_home_path = pw->pw_dir;
			}
		}
	}
	if (new_home_path == NULL) {
		home_path = NULL;
	}
	else {
		home_path = malloc(strlen(new_home_path) + 1);
		if (home_path == NULL) {
			error(MALLOC_PROBLEM);
		}
		else {
			strcpy(home_path, new_home_path);
			if (stat(home_path, &buf) == -1) {
				free(home_path);
				home_path = NULL;
				warning(HOME_STAT_PROBLEM);
			}
		}
	}

	return NO_PROBLEM;
}

/**
Finishes initializing the configuration.

@return The error code.
**/
problem_t end_init_config(void) {
	/*
	Closes the configuration file.

	The memory allocated by the <code>config_lookup_*</code> calls is automatically deallocated.
	*/
	config_destroy(&config);

	return NO_PROBLEM;
}

problem_t init_launcher_config(void) {
	PROPAGATE(init_config());

	/*
	Enables preloading libraries.

	The configuration file is first searched,
	the environment variable is then gotten,
	the existence of the file is then checked and
	the environment variable is finally set.
	*/
	const char * new_loader_path;
	if (config_lookup_string(&config, "loader", &new_loader_path) == 0) {
		//warning(LD_PRELOAD_CONFIG_PROBLEM);
		new_loader_path = getenv("LD_PRELOAD");
		if (loader_path == NULL) {
			new_loader_path = default_loader_path;
			warning(LD_PRELOAD_GETENV_PROBLEM);
		}
	}
	if (new_loader_path == NULL) {
		loader_path = NULL;
	}
	else {
		loader_path = astrrep(new_loader_path, "~", home_path);
		if (stat(loader_path, &buf) == -1) {
			free(loader_path);
			loader_path = NULL;
			return error(LD_PRELOAD_STAT_PROBLEM);
		}
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
	const char * new_executable_path;
	if (config_lookup_string(&config, "executable", &new_executable_path) == 0) {
		new_executable_path = default_executable_path;
		//warning(EXECUTABLE_CONFIG_PROBLEM);
	}
	if (new_executable_path == NULL) {
		executable_path = NULL;
	}
	else {
		executable_path = astrrep(new_executable_path, "~", home_path);
		if (stat(executable_path, &buf) == -1) {
			free(executable_path);
			executable_path = NULL;
			return error(EXECUTABLE_STAT_PROBLEM);
		}
		else if (S_ISDIR(buf.st_mode)) {
			free(executable_path);
			executable_path = NULL;
			return error(EXECUTABLE_TYPE_PROBLEM);
		}
		else if ((buf.st_mode & (S_ISUID | S_ISGID)) != 0) {
			free(executable_path);
			executable_path = NULL;
			return error(EXECUTABLE_PERMISSION_PROBLEM);
		}
		else {
			const size_t size = (size_t )buf.st_size;
			if (size != executable_size) {
				warning(EXECUTABLE_SIZE_PROBLEM);
			}
			else {
				FILE * stream = fopen(executable_path, "rb");
				if (stream == NULL) {
					error(EXECUTABLE_OPEN_PROBLEM);
				}
				else {
					unsigned char * ptr = malloc(size);
					if (ptr == NULL) {
						error(MALLOC_PROBLEM);
					}
					else {
						if (fread(ptr, 1, size, stream) != executable_size) {
							error(EXECUTABLE_READ_PROBLEM);
						}
						else if (hash(ptr, executable_size) != executable_hash) {
							warning(EXECUTABLE_HASH_PROBLEM);
						}
						free(ptr);
						if (fclose(stream) == EOF) {
							error(EXECUTABLE_CLOSE_PROBLEM);
						}
					}
				}
			}
		}
	}

	/*
	Finds the data directory path of the executable.

	The configuration file is first searched,
	the location of the directory is then guessed and
	the existence of the directory is finally checked.
	*/
	const char * new_executable_data_path;
	if (config_lookup_string(&config, "data", &new_executable_data_path) == 0) {
		new_executable_data_path = NULL;
		//warning(EXECUTABLE_DATA_CONFIG_PROBLEM);
		if (home_path == NULL) {
			executable_data_path = NULL;
		}
		else {
			const size_t size = strlen(home_path) + 1 + strlen(executable_data_file) + 1;
			executable_data_path = malloc(size);
			if (home_path == NULL) {
				executable_data_path = NULL;
				error(MALLOC_PROBLEM);
			}
			else {
				snprintf(executable_data_path, size, "%s/%s", home_path, executable_data_file);
			}
		}
	}
	else {
		executable_data_path = astrrep(new_executable_data_path, "~", home_path);
	}
	if (executable_data_path == NULL) {
		return error(NULL_PROBLEM);
	}
	else if (stat(executable_data_path, &buf) == -1) {
		free(executable_data_path);
		executable_data_path = NULL;
		return error(EXECUTABLE_DATA_STAT_PROBLEM);
	}

	/*
	Finds the temporary file directory path of the executable.

	The location of the directory is first guessed and
	the existence of the directory is then checked.
	*/
	if (executable_data_path == NULL) {
		executable_temporary_path = NULL;
	}
	else {
		const size_t size = strlen(executable_data_path) + 1 + strlen(executable_temporary_file) + 1;
		executable_temporary_path = malloc(size);
		if (executable_temporary_path == NULL) {
			error(MALLOC_PROBLEM);
		}
		else {
			snprintf(executable_temporary_path, size, "%s/%s", executable_data_path, executable_temporary_file);
			if (stat(executable_temporary_path, &buf) == -1) {
				free(executable_temporary_path);
				executable_temporary_path = NULL;
				warning(EXECUTABLE_TEMPORARY_STAT_PROBLEM);
			}
		}
	}

	/*
	Finds the config file path of the executable.
	*/
	executable_config_path = NULL;//not implemented

	/*
	Finds the process lock file path of the executable.

	The location of the file is first guessed and
	the existence of the file is then checked.
	*/
	if (executable_data_path == NULL) {
		executable_process_path = NULL;
	}
	else {
		const size_t size = strlen(executable_data_path) + 1 + strlen(executable_process_file) + 1;
		executable_process_path = malloc(size);
		if (executable_process_path == NULL) {
			error(MALLOC_PROBLEM);
		}
		else {
			snprintf(executable_process_path, size, "%s/%s", executable_data_path, executable_process_file);
			if (stat(executable_process_path, &buf) == -1) {
				free(executable_process_path);
				executable_process_path = NULL;
				//note(EXECUTABLE_PROCESS_STAT_PROBLEM);
			}
		}
	}

	/*
	Finds the keybind file path of the executable.
	*/
	executable_keybind_path = NULL;//not implemented

	/*
	Finds the version file path of the executable.

	The location of the file is first guessed and
	the existence of the file is then checked.
	*/
	if (executable_data_path == NULL) {
		executable_version_path = NULL;
	}
	else {
		const size_t size = strlen(executable_data_path) + 1 + strlen(executable_version_file) + 1;
		executable_version_path = malloc(size);
		if (executable_version_path == NULL) {
			error(MALLOC_PROBLEM);
		}
		else {
			snprintf(executable_version_path, size, "%s/%s", executable_data_path, executable_version_file);
			if (stat(executable_version_path, &buf) == -1) {
				free(executable_version_path);
				executable_version_path = NULL;
				warning(EXECUTABLE_VERSION_STAT_PROBLEM);
			}
		}
	}

	/*
	Finds the count file path of the executable.

	The location of the file is first guessed and
	the existence of the file is then checked.
	*/
	if (executable_data_path == NULL) {
		executable_count_path = NULL;
	}
	else {
		const size_t size = strlen(executable_data_path) + 1 + strlen(executable_count_file) + 1;
		executable_count_path = malloc(size);
		if (executable_count_path == NULL) {
			error(MALLOC_PROBLEM);
		}
		else {
			snprintf(executable_count_path, size, "%s/%s", executable_data_path, executable_count_file);
			if (stat(executable_count_path, &buf) == -1) {
				free(executable_count_path);
				executable_count_path = NULL;
				warning(EXECUTABLE_COUNT_STAT_PROBLEM);
			}
		}
	}

	PROPAGATE(end_init_config());

	return NO_PROBLEM;
}

problem_t init_loader_config(void) {
	PROPAGATE(init_config());

	/*
	Finds the libraries to load.

	The configuration file is first parsed,
	the environment variable is then read and
	the existence of the file is then checked.
	*/
	const char * new_libc_path;
	if (config_lookup_string(&config, "libc", &new_libc_path) == 0) {
		new_libc_path = default_libc_path;
		warning(LIBC_CONFIG_PROBLEM);
	}
	libc_path = astrrep(new_libc_path, "~", home_path);
	if (stat(libc_path, &buf) != 0) {
		free(libc_path);
		libc_path = NULL;
		return error(LIBC_STAT_PROBLEM);
	}
	const char * new_libncurses_path;
	if (config_lookup_string(&config, "libncurses", &new_libncurses_path) == 0) {
		new_libncurses_path = default_libncurses_path;
		warning(LIBNCURSES_CONFIG_PROBLEM);
	}
	libncurses_path = astrrep(new_libncurses_path, "~", home_path);
	if (stat(libncurses_path, &buf) != 0) {
		free(libncurses_path);
		libncurses_path = NULL;
		return error(LIBNCURSES_STAT_PROBLEM);
	}

	/*
	Finds the amount of save states.

	The configuration file is first parsed,
	the default amount is then assumed and
	the bounds are finally checked.
	*/
	int new_states;
	if (config_lookup_int(&config, "states", &new_states) == 0) {
		new_states = default_states;
		//note(STATE_CONFIG_PROBLEM);
	}
	if (new_states < 1) {
		new_states = 1;
		warning(STATE_AMOUNT_PROBLEM);
	}
	states = (unsigned int )new_states + 1;//reserves space for the active state

	/*
	Finds the height of the terminal.

	The configuration file is first parsed,
	the default height is then assumed and
	the bounds are finally checked.
	*/
	int new_rows;
	if (config_lookup_int(&config, "rows", &new_rows) == 0) {
		new_rows = default_rows;
		note(ROW_CONFIG_PROBLEM);
	}
	if (new_rows < executable_rows_min || new_rows > executable_rows_max) {
		new_rows = MIN(MAX(executable_rows_min, new_rows), executable_rows_max);
		warning(ROW_AMOUNT_PROBLEM);
	}
	rows = (unsigned int )new_rows;

	/*
	Finds the width of the terminal.

	The configuration file is first parsed,
	the default width is then assumed and
	the bounds are finally checked.
	*/
	int new_cols;
	if (config_lookup_int(&config, "cols", &new_cols) == 0) {
		new_cols = default_cols;
		//note(COL_CONFIG_PROBLEM);
	}
	if (new_cols < executable_cols_min || new_cols > executable_cols_max) {
		new_cols = MIN(MAX(executable_cols_min, new_cols), executable_cols_max);
		warning(COL_AMOUNT_PROBLEM);
	}
	cols = (unsigned int )new_cols;

	/*
	Finds the location of the shared memory segment.

	The configuration file is first parsed and
	the default location is then guessed.
	*/
	const char * new_shm_path;
	if (config_lookup_string(&config, "shm", &new_shm_path) == 0) {
		new_shm_path = default_shm_path;
		//note(SHM_CONFIG_PROBLEM);
	}
	shm_path = astrrep(new_shm_path, "~", home_path);
	if (stat(shm_path, &buf) != 0) {//TODO bring memory mapped files back
		free(shm_path);
		shm_path = NULL;
		return error(SHM_STAT_PROBLEM);
	}

	/*
	Finds the iterator string.

	The configuration file is first parsed and
	the default string is then assumed.
	*/
	const char * new_iterator;
	if (config_lookup_string(&config, "iterator", &new_iterator) == 0) {
		new_iterator = default_iterator;
		//note(ITERATOR_CONFIG_PROBLEM);
	}
	iterator = malloc(strlen(new_iterator) + 1);
	strcpy(iterator, new_iterator);

	/*
	Opens the input stream.

	The configuration file is first parsed and
	the existence of the input file is then checked.
	*/
	const char * new_input_path;
	if (config_lookup_string(&config, "input", &new_input_path) == 0) {
		new_input_path = default_input_path;
		warning(INPUT_CONFIG_PROBLEM);
	}
	char * input_path = astrrep(new_input_path, "~", home_path);
	if (stat(input_path, &buf) != 0) {
		warning(INPUT_STAT_PROBLEM);
	}
	else {
		input_stream = fopen(input_path, "rb");
		if (input_stream == NULL) {
			error(INPUT_OPEN_PROBLEM);
		}
	}
	free(input_path);

	/*
	Opens the output streams.

	The configuration file is first parsed,
	the existence of the output file is then checked and
	the index character is then located,
	if the index character is not found
		the file is only used for the active save state,
	otherwise
		the index character is first replaced with the corresponding save state number and
		the file is then used for that save state.
	*/
	const char * new_output_path;
	if (config_lookup_string(&config, "output", &new_output_path) == 0) {
		new_output_path = default_output_path;
		warning(OUTPUT_CONFIG_PROBLEM);
	}
	char * output_path = astrrep(new_output_path, "~", home_path);
	output_streams = malloc(states * sizeof *output_streams);
	for (unsigned int state = 1; state < states; state++) {
		const size_t size = uintlen(state) + 1;
		char * output_state = malloc(size);
		snprintf(output_state, size, "%u", state);
		char * path = astrrep(output_path, iterator, output_state);
		free(output_state);
		if (stat(path, &buf) == 0) {
			warning(OUTPUT_STAT_PROBLEM);
		}
		output_streams[state] = fopen(path, "wb");
		free(path);
		if (output_streams[state] == NULL) {
			error(OUTPUT_OPEN_PROBLEM);
		}
	}
	free(output_path);

	/**
	Sets the playback mode.
	**/
	if (config_lookup_bool(&config, "output", &autoplay) == 0) {
		autoplay = default_autoplay;
	}

	/*
	Opens the log streams.

	The configuration file is first parsed,
	the existence of the log file is then checked,
	the log file is then created and
	the log stream is finally opened.
	*/
	FILE * new_error_stream = error_stream;
	FILE * new_warning_stream = warning_stream;
	FILE * new_note_stream = note_stream;
	FILE * new_call_stream = call_stream;
	struct stat error_buf;//TODO check NULL problems and refine
	struct stat warning_buf;
	struct stat note_buf;
	struct stat call_buf;
	const char * new_error_path;
	if (config_lookup_string(&config, "errors", &new_error_path) == 0) {
		new_error_path = default_error_stream;
		warning(ERROR_CONFIG_PROBLEM);
	}
	char * error_path = astrrep(new_error_path, "~", home_path);
	new_error_stream = stdstr(error_path);
	if (new_error_stream == NULL) {
		if (stat(error_path, &buf) == 0) {
			//note(ERROR_STAT_PROBLEM);
		}
		new_error_stream = fopen(error_path, "w");
		if (new_error_stream == NULL) {
			new_error_stream = NULL;
			warning(ERROR_OPEN_PROBLEM);
		}
	}
	free(error_path);
	//buf.st_dev && buf.st_ino
	if (new_error_stream != error_stream
			|| new_warning_stream != warning_stream
			|| new_note_stream != note_stream
			|| new_call_stream != call_stream) {
		note(LOG_CHANGE_PROBLEM);
		error_stream = new_error_stream;
		warning_stream = new_warning_stream;
		note_stream = new_note_stream;
		call_stream = new_call_stream;
	}
	warning_stream = NULL;
	note_stream = NULL;
	call_stream = NULL;

	PROPAGATE(end_init_config());

	return NO_PROBLEM;
}

#endif
