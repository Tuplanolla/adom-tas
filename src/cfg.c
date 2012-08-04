/**
Manages the configuration variables.

Some redundant conditions are used to
	enforce predictable behavior and
	make maintenance (return point changes) easier.

TODO add enforce
TODO check output_paths management

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef CFG_C
#define CFG_C

#include <stdlib.h>//*alloc, free, *env, size_t, NULL
#include <stdio.h>//*open, *close, *read, *write, *print*, FILE, EOF
#include <string.h>//str*
#include <sys/stat.h>//stat, S_*
#include <pwd.h>//*id, *wd

#include <libconfig.h>//config_*, CONFIG_*

#include "util.h"//intern, hash, stdstr, SUBNULL
#include "prob.h"//problem_d, *_PROBLEM
#include "log.h"//error, warning, note
#include "def.h"//default_*
#include "exec.h"//exec_*

#include "cfg.h"

/*
Undocumented.
*/
intern char * home_path = NULL;
intern char * exec_path = NULL;
intern char * exec_data_path = NULL;
intern char * exec_temporary_path = NULL;
intern char ** exec_temporary_paths = NULL;
intern char * exec_config_path = NULL;
intern char * exec_process_path = NULL;
intern char * exec_keybind_path = NULL;
intern char * exec_version_path = NULL;
intern char * exec_error_path = NULL;
intern char * exec_count_path = NULL;
intern char * loader_path = NULL;
intern char * libc_path = NULL;
intern char * libncurses_path = NULL;
intern bool enforce;
intern int states;
intern int rows;
intern int cols;
intern char * shm_path = NULL;
intern time_t timestamp;
intern unsigned int generations;
intern bool sql;
intern bool preserve;
intern bool autoplay;
intern bool monochrome;
intern char * iterator = NULL;
intern char * input_path = NULL;
intern char ** output_paths = NULL;
intern FILE * error_stream = NULL;
intern FILE * warning_stream = NULL;
intern FILE * note_stream = NULL;
intern FILE * call_stream = NULL;
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
problem_d uninit_config(void) {
	/*
	Closes files and deallocates manually allocated memory.
	*/
	free(home_path);
	free(exec_path);
	free(exec_data_path);
	free(exec_temporary_path);
	if (exec_temporary_paths != NULL) {
		for (unsigned int level = 0; level < exec_temporary_levels; level++) {
			const unsigned int offset = level * exec_temporary_parts;
			for (unsigned int part = 0; part < exec_temporary_parts; part++) {
				const unsigned int path = offset + part;
				free(exec_temporary_paths[path]);
			}
		}
		free(exec_temporary_paths);
	}
	free(exec_config_path);
	free(exec_process_path);
	free(exec_keybind_path);
	free(exec_version_path);
	free(exec_error_path);
	free(exec_count_path);
	free(loader_path);
	free(libc_path);
	free(libncurses_path);
	free(shm_path);
	free(iterator);
	free(input_path);
	if (output_paths != NULL) {
		for (int state = 0; state < states; state++) {
			free(output_paths[state]);
		}
		free(output_paths);
	}
	if (error_stream != NULL) {
		if (fclose(error_stream) == EOF) {
			error(ERROR_CLOSE_PROBLEM);
		}
		else {
			if (warning_stream == error_stream) {
				warning_stream = NULL;
			}
			if (note_stream == error_stream) {
				note_stream = NULL;
			}
			if (call_stream == error_stream) {
				call_stream = NULL;
			}
		}
	}
	if (warning_stream != NULL) {
		if (fclose(warning_stream) == EOF) {
			error(WARNING_CLOSE_PROBLEM);
		}
		else {
			if (note_stream == warning_stream) {
				note_stream = NULL;
			}
			if (call_stream == warning_stream) {
				call_stream = NULL;
			}
		}
	}
	if (note_stream != NULL) {
		if (fclose(note_stream) == EOF) {
			error(NOTE_CLOSE_PROBLEM);
		}
		else {
			if (call_stream == note_stream) {
				call_stream = NULL;
			}
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
problem_d init_config(void) {
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
			return error(MALLOC_PROBLEM);
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
	const char * new_exec_data_path;
	if (config_lookup_string(&config, "data", &new_exec_data_path) == CONFIG_FALSE) {
		new_exec_data_path = NULL;
		//warning(EXEC_DATA_CONFIG_PROBLEM);
		if (home_path == NULL) {
			exec_data_path = NULL;
		}
		else {
			const size_t size = strlen(home_path) + 1
					+ strlen(exec_data_directory) + 1;
			exec_data_path = malloc(size);
			if (exec_data_path == NULL) {
				return error(MALLOC_PROBLEM);
			}
			else {
				snprintf(exec_data_path, size, "%s/%s",
						home_path,
						exec_data_directory);
			}
		}
	}
	else {
		exec_data_path = astrrep(new_exec_data_path, "~", home_path);
	}
	if (exec_data_path == NULL) {
		return error(NULL_PROBLEM);
	}
	else {
		struct stat exec_data_stat;
		if (stat(exec_data_path, &exec_data_stat) == -1) {
			warning(EXEC_DATA_STAT_PROBLEM);
		}
	}

	/*
	Finds the temporary file directory path of the executable.

	The location of the directory is first guessed and
	the existence of the directory is then checked.
	*/
	if (exec_data_path == NULL) {
		exec_temporary_path = NULL;
	}
	else {
		const size_t size = strlen(exec_data_path) + 1
				+ strlen(exec_temporary_directory) + 1;
		exec_temporary_path = malloc(size);
		if (exec_temporary_path == NULL) {
			return error(MALLOC_PROBLEM);
		}
		else {
			snprintf(exec_temporary_path, size, "%s/%s",
					exec_data_path,
					exec_temporary_directory);
			struct stat exec_temporary_stat;
			if (stat(exec_temporary_path, &exec_temporary_stat) == -1) {
				warning(EXEC_TEMPORARY_STAT_PROBLEM);
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
	if (exec_temporary_path == NULL) {
		exec_temporary_paths = NULL;
	}
	else {
		exec_temporary_paths = malloc(exec_temporary_levels * exec_temporary_parts * sizeof *exec_temporary_paths);
		if (exec_temporary_paths == NULL) {
			return error(MALLOC_PROBLEM);
		}
		else {
			const int level_width = uintlen(exec_temporary_levels);
			const int part_width = uintlen(exec_temporary_parts);
			for (unsigned int level = 0; level < exec_temporary_levels; level++) {
				const unsigned int offset = level * exec_temporary_parts;
				for (unsigned int part = 0; part < exec_temporary_parts; part++) {
					const unsigned int path = offset + part;
					const size_t size = strlen(exec_temporary_path) + 1
							+ strlen(exec_temporary_file)
							+ 4 + 1;
					exec_temporary_paths[path] = malloc(size);
					if (exec_temporary_paths[path] == NULL) {
						return error(MALLOC_PROBLEM);
					}
					else {
						snprintf(exec_temporary_paths[path], size, "%s/%s%0*u_%0*u",
								exec_temporary_path,
								exec_temporary_file, level_width, level, part_width, part);
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
problem_d end_init_config(void) {
	/*
	Closes the configuration file.

	The memory allocated by the <code>config_lookup_*</code> calls is automatically deallocated.
	*/
	config_destroy(&config);

	return NO_PROBLEM;
}

problem_d init_external_config(void) {
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
	const char * new_exec_path;
	if (config_lookup_string(&config, "executable", &new_exec_path) == CONFIG_FALSE) {
		new_exec_path = default_exec_path;
		//warning(EXEC_CONFIG_PROBLEM);
	}
	if (new_exec_path == NULL) {
		exec_path = NULL;
	}
	else {
		exec_path = astrrep(new_exec_path, "~", home_path);
		struct stat exec_stat;
		if (stat(exec_path, &exec_stat) == -1) {
			free(exec_path);
			exec_path = NULL;
			return error(EXEC_STAT_PROBLEM);
		}
		else if (S_ISDIR(exec_stat.st_mode)) {
			free(exec_path);
			exec_path = NULL;
			return error(EXEC_TYPE_PROBLEM);
		}
		else if ((exec_stat.st_mode & (S_ISUID | S_ISGID)) != 0) {
			free(exec_path);
			exec_path = NULL;
			return error(EXEC_PERMISSION_PROBLEM);
		}
		else {
			const size_t size = (size_t )exec_stat.st_size;
			if (size != exec_size) {
				warning(EXEC_SIZE_PROBLEM);
			}
			else {
				FILE * stream = fopen(exec_path, "rb");
				if (stream == NULL) {
					error(EXEC_OPEN_PROBLEM);
				}
				else {
					unsigned char * ptr = malloc(size);
					if (ptr == NULL) {
						return error(MALLOC_PROBLEM);
					}
					else {
						if (fread(ptr, 1, size, stream) != exec_size) {
							error(EXEC_READ_PROBLEM);
						}
						else if (hash(ptr, exec_size) != exec_hash) {
							warning(EXEC_HASH_PROBLEM);
						}
						free(ptr);
						if (fclose(stream) == EOF) {
							error(EXEC_CLOSE_PROBLEM);
						}
					}
				}
			}
		}
	}

	/*
	Finds the config file path of the executable.

	The location of the file is first guessed and
	the existence of the file is then checked.
	*/
	if (exec_data_path == NULL) {
		exec_config_path = NULL;
	}
	else {
		const size_t size = strlen(exec_data_path) + 1
				+ strlen(exec_config_file) + 1;
		exec_config_path = malloc(size);
		if (exec_config_path == NULL) {
			return error(MALLOC_PROBLEM);
		}
		else {
			snprintf(exec_config_path, size, "%s/%s",
					exec_data_path,
					exec_config_file);
			struct stat exec_config_stat;
			if (stat(exec_config_path, &exec_config_stat) == -1) {
				warning(EXEC_CONFIG_STAT_PROBLEM);
			}
		}
	}

	/*
	Finds the process lock file path of the executable.

	The location of the file is first guessed and
	the existence of the file is then checked.
	*/
	if (exec_data_path == NULL) {
		exec_process_path = NULL;
	}
	else {
		const size_t size = strlen(exec_data_path) + 1
				+ strlen(exec_process_file) + 1;
		exec_process_path = malloc(size);
		if (exec_process_path == NULL) {
			return error(MALLOC_PROBLEM);
		}
		else {
			snprintf(exec_process_path, size, "%s/%s",
					exec_data_path,
					exec_process_file);
			struct stat exec_process_stat;
			if (stat(exec_process_path, &exec_process_stat) == -1) {
				//note(EXEC_PROCESS_STAT_PROBLEM);
			}
		}
	}

	/*
	Finds the keybind file path of the executable.

	The location of the file is first guessed and
	the existence of the file is then checked.
	*/
	if (exec_data_path == NULL) {
		exec_keybind_path = NULL;
	}
	else {
		const size_t size = strlen(exec_data_path) + 1
				+ strlen(exec_keybind_file) + 1;
		exec_keybind_path = malloc(size);
		if (exec_keybind_path == NULL) {
			return error(MALLOC_PROBLEM);
		}
		else {
			snprintf(exec_keybind_path, size, "%s/%s",
					exec_data_path,
					exec_keybind_file);
			struct stat exec_keybind_stat;
			if (stat(exec_keybind_path, &exec_keybind_stat) == -1) {
				warning(EXEC_KEYBIND_STAT_PROBLEM);
			}
		}
	}

	/*
	Finds the version file path of the executable.

	The location of the file is first guessed and
	the existence of the file is then checked.
	*/
	if (exec_data_path == NULL) {
		exec_version_path = NULL;
	}
	else {
		const size_t size = strlen(exec_data_path) + 1
				+ strlen(exec_version_file) + 1;
		exec_version_path = malloc(size);
		if (exec_version_path == NULL) {
			return error(MALLOC_PROBLEM);
		}
		else {
			snprintf(exec_version_path, size, "%s/%s",
					exec_data_path,
					exec_version_file);
			struct stat exec_version_stat;
			if (stat(exec_version_path, &exec_version_stat) == -1) {
				warning(EXEC_VERSION_STAT_PROBLEM);
			}
		}
	}

	/*
	Finds the error log file path of the executable.

	The location of the file is first guessed and
	the existence of the file is then checked.
	*/
	if (exec_data_path == NULL) {
		exec_error_path = NULL;
	}
	else {
		const size_t size = strlen(exec_data_path) + 1
				+ strlen(exec_error_file) + 1;
		exec_error_path = malloc(size);
		if (exec_error_path == NULL) {
			return error(MALLOC_PROBLEM);
		}
		else {
			snprintf(exec_error_path, size, "%s/%s",
					exec_data_path,
					exec_error_file);
			struct stat exec_error_stat;
			if (stat(exec_error_path, &exec_error_stat) == -1) {
				//note(EXEC_ERROR_STAT_PROBLEM);
			}
		}
	}

	/*
	Finds the count file path of the executable.

	The location of the file is first guessed and
	the existence of the file is then checked.
	*/
	if (exec_data_path == NULL) {
		exec_count_path = NULL;
	}
	else {
		const size_t size = strlen(exec_data_path) + 1
				+ strlen(exec_count_file) + 1;
		exec_count_path = malloc(size);
		if (exec_count_path == NULL) {
			return error(MALLOC_PROBLEM);
		}
		else {
			snprintf(exec_count_path, size, "%s/%s",
					exec_data_path,
					exec_count_file);
			struct stat exec_count_stat;
			if (stat(exec_count_path, &exec_count_stat) == -1) {
				warning(EXEC_COUNT_STAT_PROBLEM);
			}
		}
	}

	PROPAGATE(end_init_config());

	return NO_PROBLEM;
}

problem_d init_internal_config(void) {
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

	/**
	Sets the save state perseverance.
	**/
	int new_enforce;
	if (config_lookup_bool(&config, "enforce", &new_enforce) == CONFIG_FALSE) {
		new_enforce = default_enforce;
		//warning(PRESERVE_CONFIG_PROBLEM);
	}
	enforce = new_enforce == CONFIG_TRUE;

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
	states = new_states + 1;//reserves space for the active state

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
	if (new_rows < exec_rows_min || new_rows > exec_rows_max) {
		new_rows = MIN(MAX(exec_rows_min, new_rows), exec_rows_max);
		warning(ROW_AMOUNT_PROBLEM);
	}
	rows = new_rows;

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
	if (new_cols < exec_cols_min || new_cols > exec_cols_max) {
		new_cols = MIN(MAX(exec_cols_min, new_cols), exec_cols_max);
		warning(COL_AMOUNT_PROBLEM);
	}
	cols = new_cols;

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
	if (config_lookup_int(&config, "generations", &new_generations) == CONFIG_FALSE) {
		new_generations = default_generations;
		//warning(GENERATIONS_CONFIG_PROBLEM);
	}
	generations = (unsigned int )new_generations;

	/**
	Sets the save-quit-load emulation.
	**/
	int new_sql;
	if (config_lookup_bool(&config, "sql", &new_sql) == CONFIG_FALSE) {
		new_sql = default_sql;
		//warning(SQL_CONFIG_PROBLEM);
	}
	sql = new_sql == CONFIG_TRUE;

	/**
	Sets the save state perseverance.
	**/
	int new_preserve_states;
	if (config_lookup_bool(&config, "preserve", &new_preserve_states) == CONFIG_FALSE) {
		new_preserve_states = default_preserve;
		//warning(PRESERVE_CONFIG_PROBLEM);
	}
	preserve = new_preserve_states == CONFIG_TRUE;

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
	int new_monochrome;
	if (config_lookup_bool(&config, "monochrome", &new_monochrome) == CONFIG_FALSE) {
		new_monochrome = default_monochrome;
		//warning(MONOCHROME_CONFIG_PROBLEM);
	}
	monochrome = new_monochrome == CONFIG_TRUE;

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
	output_paths = malloc((size_t )states * sizeof *output_paths);
	if (output_paths == NULL) {
		return error(MALLOC_PROBLEM);
	}
	else {
		bool exists = FALSE;
		for (int state = 1; state < states; state++) {
			const size_t size = intlen(state) + 1;
			char * const iterand = malloc(size);
			if (iterand == NULL) {
				output_paths = NULL;
				return error(MALLOC_PROBLEM);
			}
			else {
				snprintf(iterand, size, "%d", state);
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
	if (config_lookup_string(&config, "error_log", &new_error_path) == CONFIG_FALSE) {
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
	if (config_lookup_string(&config, "warning_log", &new_warning_path) == CONFIG_FALSE) {
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
	if (config_lookup_string(&config, "note_log", &new_note_path) == CONFIG_FALSE) {
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
	if (config_lookup_string(&config, "call_log", &new_call_path) == CONFIG_FALSE) {
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

	/**
	Sets the keys.
	**/
	int new_save_key;
	if (config_lookup_int(&config, "save_key", &new_save_key) == CONFIG_FALSE) {
		new_save_key = default_save_key;
		//warning(KEY_CONFIG_PROBLEM);
	}
	save_key = new_save_key;

	int new_load_key;
	if (config_lookup_int(&config, "load_key", &new_load_key) == CONFIG_FALSE) {
		new_load_key = default_load_key;
		//warning(KEY_CONFIG_PROBLEM);
	}
	load_key = new_load_key;

	int new_state_key;
	if (config_lookup_int(&config, "state_key", &new_state_key) == CONFIG_FALSE) {
		new_state_key = default_state_key;
		//warning(KEY_CONFIG_PROBLEM);
	}
	state_key = new_state_key;

	int new_unstate_key;
	if (config_lookup_int(&config, "unstate_key", &new_unstate_key) == CONFIG_FALSE) {
		new_unstate_key = default_unstate_key;
		//warning(KEY_CONFIG_PROBLEM);
	}
	unstate_key = new_unstate_key;

	int new_duration_key;
	if (config_lookup_int(&config, "duration_key", &new_duration_key) == CONFIG_FALSE) {
		new_duration_key = default_duration_key;
		//warning(KEY_CONFIG_PROBLEM);
	}
	duration_key = new_duration_key;

	int new_unduration_key;
	if (config_lookup_int(&config, "unduration_key", &new_unduration_key) == CONFIG_FALSE) {
		new_unduration_key = default_unduration_key;
		//warning(KEY_CONFIG_PROBLEM);
	}
	unduration_key = new_unduration_key;

	int new_time_key;
	if (config_lookup_int(&config, "time_key", &new_time_key) == CONFIG_FALSE) {
		new_time_key = default_time_key;
		//warning(KEY_CONFIG_PROBLEM);
	}
	time_key = new_time_key;

	int new_untime_key;
	if (config_lookup_int(&config, "untime_key", &new_untime_key) == CONFIG_FALSE) {
		new_untime_key = default_untime_key;
		//warning(KEY_CONFIG_PROBLEM);
	}
	untime_key = new_untime_key;

	int new_menu_key;
	if (config_lookup_int(&config, "menu_key", &new_menu_key) == CONFIG_FALSE) {
		new_menu_key = default_menu_key;
		//warning(KEY_CONFIG_PROBLEM);
	}
	menu_key = new_menu_key;

	int new_condense_key;
	if (config_lookup_int(&config, "condense_key", &new_condense_key) == CONFIG_FALSE) {
		new_condense_key = default_condense_key;
		//warning(KEY_CONFIG_PROBLEM);
	}
	condense_key = new_condense_key;

	int new_hide_key;
	if (config_lookup_int(&config, "hide_key", &new_hide_key) == CONFIG_FALSE) {
		new_hide_key = default_hide_key;
		//warning(KEY_CONFIG_PROBLEM);
	}
	hide_key = new_hide_key;

	int new_play_key;
	if (config_lookup_int(&config, "play_key", &new_play_key) == CONFIG_FALSE) {
		new_play_key = default_play_key;
		//warning(KEY_CONFIG_PROBLEM);
	}
	play_key = new_play_key;

	int new_stop_key;
	if (config_lookup_int(&config, "stop_key", &new_stop_key) == CONFIG_FALSE) {
		new_stop_key = default_stop_key;
		//warning(KEY_CONFIG_PROBLEM);
	}
	stop_key = new_stop_key;

	int new_quit_key;
	if (config_lookup_int(&config, "quit_key", &new_quit_key) == CONFIG_FALSE) {
		new_quit_key = default_quit_key;
		//warning(KEY_CONFIG_PROBLEM);
	}
	quit_key = new_quit_key;

	PROPAGATE(end_init_config());

	return NO_PROBLEM;
}

#endif
