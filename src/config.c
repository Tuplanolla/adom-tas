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

#include <libconfig.h>//config_*, CONFIG_*

#include "util.h"//intern, hash, stdstr, SUBNULL
#include "problem.h"//problem_t, *_PROBLEM
#include "log.h"//error, warning, note
#include "def.h"//default_*
#include "exec.h"//executable_*

#include "config.h"

/**
The configuration variables.
**/
intern char * home_path;
intern char * executable_path;
intern char * executable_data_path;
intern char * executable_temporary_path;
intern char ** executable_temporary_paths;
intern char * executable_config_path;
intern char * executable_process_path;
intern char * executable_keybind_path;
intern char * executable_version_path;
intern char * executable_count_path;
intern char * loader_path;
intern char * libc_path;
intern char * libncurses_path;
intern unsigned int states;
intern unsigned int rows;
intern unsigned int cols;
intern char * shm_path;
intern time_t timestamp;
intern unsigned int generations;
intern bool sql;
intern bool autoplay;
intern bool color;
intern char * iterator;
intern char * input_path;
intern char ** output_paths;
intern FILE * error_stream;
intern FILE * warning_stream;
intern FILE * note_stream;
intern FILE * call_stream;
intern int save_key;
intern int load_key;
intern int state_key;
intern int unstate_key;
intern int duration_key;
intern int unduration_key;
intern int time_key;
intern int untime_key;
intern int menu_key;
intern int condense_key;
intern int hide_key;
intern int play_key;
intern int stop_key;
intern int quit_key;

/**
The configuration.
**/
config_t config;

/**
Uninitializes the configuration variables.

@return The error code.
**/
problem_t uninit_config(void) {
	/*
	Closes files and deallocates manually allocated memory.
	*/
	free(home_path);
	free(executable_path);
	free(executable_data_path);
	free(executable_temporary_path);
	for (unsigned int level = 0; level < executable_temporary_levels; level++) {
		const unsigned int offset = level * executable_temporary_parts;
		for (unsigned int part = 0; part < executable_temporary_parts; part++) {
			const unsigned int path = offset + part;
			free(executable_temporary_paths[path]);
		}
	}
	free(executable_temporary_paths);
	free(executable_config_path);
	free(executable_process_path);
	free(executable_keybind_path);
	free(executable_version_path);
	free(executable_count_path);
	free(loader_path);
	free(libc_path);
	free(libncurses_path);
	free(shm_path);
	free(iterator);
	free(input_path);
	for (unsigned int state = 0; state < states; state++) {
		free(output_paths[state]);
	}
	free(output_paths);
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
	struct stat config_stat;
	if (stat(default_config_path, &config_stat) == -1) {
		FILE * stream = fopen(default_config_path, "w");
		if (stream == NULL) {
			return error(CONFIGURATION_OPEN_PROBLEM);
		}
		else {
			if ((size_t )fprintf(stream, "%s", default_config) != strlen(default_config)) {
				error(CONFIGURATION_WRITE_PROBLEM);
			}
			if (fclose(stream) == EOF) {
				error(CONFIGURATION_CLOSE_PROBLEM);
			}
		}
		return note(CONFIGURATION_STAT_PROBLEM);
	}
	config_init(&config);
	if (config_read_file(&config, default_config_path) == 0) {
		return error(CONFIGURATION_PARSE_PROBLEM);
	}

	/*
	Finds the home directory.

	The configuration file is first parsed,
	the environment variable is then read,
	a system call then is used and
	the existence of the directory is finally checked.
	*/
	const char * new_home_path;
	struct passwd * pw;
	if (config_lookup_string(&config, "home", &new_home_path) == CONFIG_FALSE) {
		//warning(HOME_CONFIGURATION_PROBLEM);
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
			struct stat home_stat;
			if (stat(home_path, &home_stat) == -1) {
				free(home_path);
				home_path = NULL;
				warning(HOME_STAT_PROBLEM);
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
	if (config_lookup_string(&config, "data", &new_executable_data_path) == CONFIG_FALSE) {
		new_executable_data_path = NULL;
		//warning(EXECUTABLE_DATA_CONFIG_PROBLEM);
		if (home_path == NULL) {
			executable_data_path = NULL;
		}
		else {
			const size_t size = strlen(home_path) + 1
					+ strlen(executable_data_directory) + 1;
			executable_data_path = malloc(size);
			if (executable_data_path == NULL) {
				error(MALLOC_PROBLEM);
			}
			else {
				snprintf(executable_data_path, size, "%s/%s",
						home_path,
						executable_data_directory);
			}
		}
	}
	else {
		executable_data_path = astrrep(new_executable_data_path, "~", home_path);
	}
	if (executable_data_path == NULL) {
		return error(NULL_PROBLEM);
	}
	else {
		struct stat executable_data_stat;
		if (stat(executable_data_path, &executable_data_stat) == -1) {
			warning(EXECUTABLE_DATA_STAT_PROBLEM);
		}
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
		const size_t size = strlen(executable_data_path) + 1
				+ strlen(executable_temporary_directory) + 1;
		executable_temporary_path = malloc(size);
		if (executable_temporary_path == NULL) {
			error(MALLOC_PROBLEM);
		}
		else {
			snprintf(executable_temporary_path, size, "%s/%s",
					executable_data_path,
					executable_temporary_directory);
			struct stat executable_temporary_stat;
			if (stat(executable_temporary_path, &executable_temporary_stat) == -1) {
				warning(EXECUTABLE_TEMPORARY_STAT_PROBLEM);
			}
		}
	}

	/*
	Finds the temporary file paths of the executable.

	The executable uses the format <code>"%s%0*d_%d"</code>.

	The location of the directory is first guessed,
	the existence of the directory is then checked and
	the file paths are finally guessed.
	*/
	if (executable_temporary_path == NULL) {
		executable_temporary_paths = NULL;
	}
	else {
		executable_temporary_paths = malloc(executable_temporary_levels * executable_temporary_parts * sizeof *executable_temporary_paths);
		if (executable_temporary_paths == NULL) {
			error(MALLOC_PROBLEM);
		}
		else {
			for (unsigned int level = 0; level < executable_temporary_levels; level++) {
				const unsigned int offset = level * executable_temporary_parts;
				for (unsigned int part = 0; part < executable_temporary_parts; part++) {
					const unsigned int path = offset + part;
					const size_t size = strlen(executable_temporary_path) + 1
							+ strlen(executable_temporary_file)
							+ 4 + 1;
					executable_temporary_paths[path] = malloc(size);
					if (executable_temporary_paths[path] == NULL) {
						error(MALLOC_PROBLEM);
					}
					else {
						snprintf(executable_temporary_paths[path], size, "%s/%s%02u_%01u",
								executable_temporary_path,
								executable_temporary_file,
								level,
								part);
					}
				}
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
	if (config_lookup_string(&config, "loader", &new_loader_path) == CONFIG_FALSE) {
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
		struct stat loader_stat;
		if (stat(loader_path, &loader_stat) == -1) {
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
	if (config_lookup_string(&config, "executable", &new_executable_path) == CONFIG_FALSE) {
		new_executable_path = default_executable_path;
		//warning(EXECUTABLE_CONFIG_PROBLEM);
	}
	if (new_executable_path == NULL) {
		executable_path = NULL;
	}
	else {
		executable_path = astrrep(new_executable_path, "~", home_path);
		struct stat executable_stat;
		if (stat(executable_path, &executable_stat) == -1) {
			free(executable_path);
			executable_path = NULL;
			return error(EXECUTABLE_STAT_PROBLEM);
		}
		else if (S_ISDIR(executable_stat.st_mode)) {
			free(executable_path);
			executable_path = NULL;
			return error(EXECUTABLE_TYPE_PROBLEM);
		}
		else if ((executable_stat.st_mode & (S_ISUID | S_ISGID)) != 0) {
			free(executable_path);
			executable_path = NULL;
			return error(EXECUTABLE_PERMISSION_PROBLEM);
		}
		else {
			const size_t size = (size_t )executable_stat.st_size;
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
		const size_t size = strlen(executable_data_path) + 1
				+ strlen(executable_process_file) + 1;
		executable_process_path = malloc(size);
		if (executable_process_path == NULL) {
			error(MALLOC_PROBLEM);
		}
		else {
			snprintf(executable_process_path, size, "%s/%s",
					executable_data_path,
					executable_process_file);
			struct stat executable_process_stat;
			if (stat(executable_process_path, &executable_process_stat) == -1) {
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
		const size_t size = strlen(executable_data_path) + 1
				+ strlen(executable_version_file) + 1;
		executable_version_path = malloc(size);
		if (executable_version_path == NULL) {
			error(MALLOC_PROBLEM);
		}
		else {
			snprintf(executable_version_path, size, "%s/%s",
					executable_data_path,
					executable_version_file);
			struct stat executable_version_stat;
			if (stat(executable_version_path, &executable_version_stat) == -1) {
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
		const size_t size = strlen(executable_data_path) + 1
				+ strlen(executable_count_file) + 1;
		executable_count_path = malloc(size);
		if (executable_count_path == NULL) {
			error(MALLOC_PROBLEM);
		}
		else {
			snprintf(executable_count_path, size, "%s/%s",
					executable_data_path,
					executable_count_file);
			struct stat executable_count_stat;
			if (stat(executable_count_path, &executable_count_stat) == -1) {
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
	if (config_lookup_string(&config, "libc", &new_libc_path) == CONFIG_FALSE) {
		new_libc_path = default_libc_path;
		warning(LIBC_CONFIG_PROBLEM);
	}
	libc_path = astrrep(new_libc_path, "~", home_path);
	struct stat libc_stat;
	if (stat(libc_path, &libc_stat) == -1) {
		free(libc_path);
		libc_path = NULL;
		return error(LIBC_STAT_PROBLEM);
	}

	const char * new_libncurses_path;
	if (config_lookup_string(&config, "libncurses", &new_libncurses_path) == CONFIG_FALSE) {
		new_libncurses_path = default_libncurses_path;
		warning(LIBNCURSES_CONFIG_PROBLEM);
	}
	libncurses_path = astrrep(new_libncurses_path, "~", home_path);
	struct stat libncurses_stat;
	if (stat(libncurses_path, &libncurses_stat) != 0) {
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
	if (config_lookup_int(&config, "states", &new_states) == CONFIG_FALSE) {
		new_states = default_states;
		//warning(STATE_CONFIG_PROBLEM);
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
	if (config_lookup_int(&config, "rows", &new_rows) == CONFIG_FALSE) {
		new_rows = default_rows;
		//warning(ROW_CONFIG_PROBLEM);
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
	if (config_lookup_int(&config, "cols", &new_cols) == CONFIG_FALSE) {
		new_cols = default_cols;
		//warning(COL_CONFIG_PROBLEM);
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
	if (config_lookup_string(&config, "shm", &new_shm_path) == CONFIG_FALSE) {
		new_shm_path = default_config_path;//default_shm_path;
		//warning(SHM_CONFIG_PROBLEM);
	}
	shm_path = astrrep(new_shm_path, "~", home_path);
	struct stat shm_stat;
	if (stat(shm_path, &shm_stat) == -1) {
		free(shm_path);
		shm_path = NULL;
		return error(SHM_STAT_PROBLEM);
	}

	/**
	Sets the initial system time.
	**/
	int new_timestamp;
	if (config_lookup_int(&config, "timestamp", &new_timestamp) == CONFIG_FALSE) {
		new_timestamp = default_timestamp;
		//warning(TIMESTAMP_CONFIG_PROBLEM);
	}
	timestamp = (time_t )new_timestamp;

	/**
	Sets the amount of generated characters.
	**/
	int new_generations;
	if (config_lookup_bool(&config, "generations", &new_generations) == CONFIG_FALSE) {
		new_generations = default_generations;
		//warning(GENERATIONS_CONFIG_PROBLEM);
	}
	generations = (unsigned int )new_generations;

	/**
	Toggles the save-quit-load emulation.
	**/
	int new_sql;
	if (config_lookup_bool(&config, "sql", &new_sql) == CONFIG_FALSE) {
		new_sql = default_sql;
		//warning(SQL_CONFIG_PROBLEM);
	}
	sql = new_sql == CONFIG_TRUE;

	/**
	Sets the playback mode.
	**/
	int new_autoplay;
	if (config_lookup_bool(&config, "autoplay", &new_autoplay) == CONFIG_FALSE) {
		new_autoplay = default_autoplay;
		//warning(AUTOPLAY_CONFIG_PROBLEM);
	}
	autoplay = new_autoplay == CONFIG_TRUE;

	/**
	Sets the color mode.
	**/
	int new_color;
	if (config_lookup_bool(&config, "color", &new_color) == CONFIG_FALSE) {
		new_color = default_color;
		//warning(COLOR_CONFIG_PROBLEM);
	}
	color = new_color == CONFIG_TRUE;

	/*
	Finds the iterator string.

	The configuration file is first parsed and
	the default string is then assumed.
	*/
	const char * new_iterator;
	if (config_lookup_string(&config, "iterator", &new_iterator) == CONFIG_FALSE) {
		new_iterator = default_iterator;
		//warning(ITERATOR_CONFIG_PROBLEM);
	}
	iterator = malloc(strlen(new_iterator) + 1);
	strcpy(iterator, new_iterator);

	/*
	Opens the input stream.

	The configuration file is first parsed and
	the existence of the input file is then checked.
	*/
	const char * new_input_path;
	if (config_lookup_string(&config, "input", &new_input_path) == CONFIG_FALSE) {
		new_input_path = default_input_path;
		warning(INPUT_CONFIG_PROBLEM);
	}
	input_path = astrrep(new_input_path, "~", home_path);
	struct stat input_stat;
	if (stat(input_path, &input_stat) == -1) {
		free(input_path);
		input_path = NULL;
		warning(INPUT_STAT_PROBLEM);
	}

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
	if (config_lookup_string(&config, "output", &new_output_path) == CONFIG_FALSE) {
		new_output_path = default_output_path;
		warning(OUTPUT_CONFIG_PROBLEM);
	}
	char * const output_path = astrrep(new_output_path, "~", home_path);
	output_paths = malloc(states * sizeof *output_paths);
	if (output_paths == NULL) {
		error(MALLOC_PROBLEM);
	}
	else {
		bool exists = FALSE;
		for (unsigned int state = 1; state < states; state++) {
			const size_t size = uintlen(state) + 1;
			char * const iterand = malloc(size);
			if (iterand == NULL) {
				error(MALLOC_PROBLEM);
			}
			else {
				snprintf(iterand, size, "%u", state);
				output_paths[state] = astrrep(output_path, iterator, iterand);
				free(iterand);
				struct stat output_stat;
				if (stat(output_paths[state], &output_stat) == 0) {
					exists = TRUE;
				}
			}
		}
		if (exists) {
			warning(OUTPUT_STAT_PROBLEM);
		}
	}

	/*
	Opens the log streams.

	The configuration file is first parsed,
	the existence of the log file is then checked,
	the log file is then created and
	the log stream is finally opened.
	*/
	const char * new_error_path;
	if (config_lookup_string(&config, "errors", &new_error_path) == CONFIG_FALSE) {
		new_error_path = default_error_stream;
		warning(ERROR_CONFIG_PROBLEM);
	}
	char * const error_path = astrrep(new_error_path, "~", home_path);
	FILE * new_error_stream = stdstr(error_path);
	struct stat error_stat;
	if (new_error_stream == NULL) {
		if (stat(error_path, &error_stat) == 0) {
			//note(ERROR_STAT_PROBLEM);
		}
		new_error_stream = fopen(error_path, "w");
		if (new_error_stream == NULL) {
			new_error_stream = NULL;
			error(ERROR_OPEN_PROBLEM);
		}
	}
	free(error_path);

	const char * new_warning_path;
	if (config_lookup_string(&config, "warnings", &new_warning_path) == CONFIG_FALSE) {
		new_warning_path = default_warning_stream;
		warning(ERROR_CONFIG_PROBLEM);
	}
	char * const warning_path = astrrep(new_warning_path, "~", home_path);
	FILE * new_warning_stream = stdstr(warning_path);
	struct stat warning_stat;
	if (new_warning_stream == NULL) {
		if (stat(warning_path, &warning_stat) == 0) {
			//note(WARNING_STAT_PROBLEM);
		}
		if (warning_stat.st_dev == error_stat.st_dev
				&& warning_stat.st_ino == error_stat.st_ino) {
			new_warning_stream = new_error_stream;
		}
		else {
			new_warning_stream = fopen(warning_path, "w");
			if (new_warning_stream == NULL) {
				new_warning_stream = NULL;
				error(WARNING_OPEN_PROBLEM);
			}
		}
	}
	free(warning_path);

	const char * new_note_path;
	if (config_lookup_string(&config, "notes", &new_note_path) == CONFIG_FALSE) {
		new_note_path = default_note_stream;
		warning(ERROR_CONFIG_PROBLEM);
	}
	char * const note_path = astrrep(new_note_path, "~", home_path);
	FILE * new_note_stream = stdstr(note_path);
	struct stat note_stat;
	if (new_note_stream == NULL) {
		if (stat(note_path, &note_stat) == 0) {
			//note(NOTE_STAT_PROBLEM);
		}
		if (note_stat.st_dev == error_stat.st_dev
				&& note_stat.st_ino == error_stat.st_ino) {
			new_note_stream = new_error_stream;
		}
		else if (note_stat.st_dev == warning_stat.st_dev
				&& note_stat.st_ino == warning_stat.st_ino) {
			new_note_stream = new_warning_stream;
		}
		else {
			new_note_stream = fopen(note_path, "w");
			if (new_note_stream == NULL) {
				new_note_stream = NULL;
				error(NOTE_OPEN_PROBLEM);
			}
		}
	}
	free(note_path);

	const char * new_call_path;
	if (config_lookup_string(&config, "calls", &new_call_path) == CONFIG_FALSE) {
		new_call_path = default_call_stream;
		warning(ERROR_CONFIG_PROBLEM);
	}
	char * const call_path = astrrep(new_call_path, "~", home_path);
	FILE * new_call_stream = stdstr(call_path);
	struct stat call_stat;
	if (new_call_stream == NULL) {
		if (stat(call_path, &call_stat) == 0) {
			//note(CALL_STAT_PROBLEM);
		}
		if (call_stat.st_dev == error_stat.st_dev
				&& call_stat.st_ino == error_stat.st_ino) {
			new_call_stream = new_error_stream;
		}
		else if (call_stat.st_dev == warning_stat.st_dev
				&& call_stat.st_ino == warning_stat.st_ino) {
			new_call_stream = new_warning_stream;
		}
		else if (call_stat.st_dev == note_stat.st_dev
				&& call_stat.st_ino == note_stat.st_ino) {
			new_call_stream = new_note_stream;
		}
		else {
			new_call_stream = fopen(call_path, "w");
			if (new_call_stream == NULL) {
				new_call_stream = NULL;
				error(CALL_OPEN_PROBLEM);
			}
		}
	}
	free(call_path);
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

	save_key = default_save_key;//TODO read keys
	load_key = default_load_key;
	state_key = default_state_key;
	unstate_key = default_unstate_key;
	duration_key = default_duration_key;
	unduration_key = default_unduration_key;
	time_key = default_time_key;
	untime_key = default_untime_key;
	menu_key = default_menu_key;
	condense_key = default_condense_key;
	hide_key = default_hide_key;
	play_key = default_play_key;
	stop_key = default_stop_key;
	quit_key = default_quit_key;

	PROPAGATE(end_init_config());

	return NO_PROBLEM;
}

#endif
