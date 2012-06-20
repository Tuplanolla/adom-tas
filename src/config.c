/**
Manages various variables.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef CONFIG_C
#define CONFIG_C

#include <sys/stat.h>//stat, S_*

#include "def.h"//default_*
#include "adom.h"//executable_*
#include "problem.h"//problem_t, *_PROBLEM

#undef STDSTR//TODO get rid of this mess
#define STDSTR(stream, str) \
	if (strcmp(str, "null") == 0) stream = NULL;\
	else if (strcmp(str, "stdin") == 0) stream = stdin;\
	else if (strcmp(str, "stdout") == 0) stream = stdout;\
	else if (strcmp(str, "stderr") == 0) stream = stderr;

char * home_path;
char * executable_path;
char * executable_data_path;
char * executable_process_path;
char * executable_version_path;
char * executable_count_path;
char * executable_keybinding_path;
char * executable_config_path;
char * loader_path;
char * libc_path;
char * libncurses_path;
int generations;
int states;
int rows;
int cols;
char * iterator;
FILE * input_stream;
FILE ** output_streams;
char * shm_path;
FILE * error_stream;
FILE * warning_stream;
FILE * note_stream;
FILE * call_stream;

config_t config;
struct stat buf;

problem_t init_common_config() {
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
	if (stat(default_config_path, &buf) != 0) {//TODO check if stat accepts NULL
		FILE * stream = fopen(default_config_path, "w");
		if (stream == NULL) {
			return error(CONFIG_OPEN_PROBLEM);
		}
		else {
			if (fprintf(stream, default_config) != strlen(default_config)) {
				error(CONFIG_WRITE_PROBLEM);
			}
			if (fclose(stream) != 0) {
				error(CONFIG_CLOSE_PROBLEM);
			}
		}
		return note(CONFIG_STAT_PROBLEM);
	}
	config_init(&config);
	if (config_read_file(&config, default_config_path) == 0) {
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
	if (config_lookup_string(&config, "home", &home_path) == 0) {
		//warning(HOME_CONFIG_PROBLEM);
		home_path = getenv("HOME");
		if (home_path == NULL) {
			warning(HOME_GETENV_PROBLEM);
			struct passwd * pw = getpwuid(getuid());
			if (pw == NULL) {
				warning(HOME_GETPWUID_PROBLEM);
				home_path = default_home_path;
			}
			else {
				home_path = pw->pw_dir;
			}
		}
	}
	if (stat(home_path, &buf) != 0) {
		warning(HOME_STAT_PROBLEM);
		home_path = NULL;
	}

	return NO_PROBLEM;
}

problem_t init_launcher_config() {
	PROPAGATE(init_common_config());

	/*
	Enables preloading libraries.

	The configuration file is first searched,
	the environment variable is then gotten,
	the existence of the file is then checked and
	the environment variable is finally set.
	*/
	if (config_lookup_string(&config, "loader", &loader_path) == 0) {
		//warning(LD_PRELOAD_CONFIG_PROBLEM);
		loader_path = getenv("LD_PRELOAD");
		if (loader_path == NULL) {
			warning(LD_PRELOAD_GETENV_PROBLEM);
			loader_path = default_loader_path;
		}
	}
	if (stat(loader_path, &buf) != 0) {
		return error(LD_PRELOAD_STAT_PROBLEM);
	}
	else if (setenv("LD_PRELOAD", loader_path, TRUE) != 0) {
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
	if (config_lookup_string(&config, "executable", &executable_path) == 0) {
		warning(EXECUTABLE_CONFIG_PROBLEM);
		executable_path = default_executable_path;
	}
	if (stat(executable_path, &buf) != 0) {
		return error(EXECUTABLE_STAT_PROBLEM);
	}
	else if (S_ISDIR(buf.st_mode)) {
		return error(EXECUTABLE_TYPE_PROBLEM);
	}
	else if ((buf.st_mode & (S_ISUID | S_ISGID)) != 0) {
		return error(EXECUTABLE_PERMISSION_PROBLEM);
	}
	else {
		const size_t executable_size = buf.st_size;
		if (executable_size != executable_size) {
			warning(EXECUTABLE_SIZE_PROBLEM);
		}
		FILE * stream = fopen(executable_path, "rb");
		if (stream == NULL) {
			error(EXECUTABLE_OPEN_PROBLEM);
		}
		else {
			unsigned char * ptr = malloc(executable_size);
			const size_t size = fread(ptr, 1, executable_size, stream);
			if (size != executable_size) {
				error(EXECUTABLE_READ_PROBLEM);
			}
			else if (hash(ptr, executable_size) != executable_hash) {
				warning(EXECUTABLE_HASH_PROBLEM);
			}
			free(ptr);
			if (fclose(stream) != 0) {
				error(EXECUTABLE_CLOSE_PROBLEM);
			}
		}
	}

	/*
	Finds the data directory path of the executable.

	The configuration file is first searched,
	the location of the directory is then guessed and
	the existence of the directory is finally checked.
	*/
	if (config_lookup_string(&config, "data", &executable_data_path) == 0) {
		//warning(EXECUTABLE_DATA_CONFIG_PROBLEM);
		if (home_path == NULL) {
			error(EXECUTABLE_DATA_HOME_PROBLEM);
			executable_data_path = NULL;
		}
		else {
			const size_t size = strlen(home_path)+1+strlen(executable_data_file)+1;
			executable_data_path = malloc(size);
			snprintf(executable_data_path, size, "%s/%s", home_path, executable_data_file);
		}
	}
	if (stat(executable_data_path, &buf) != 0) {
		error(EXECUTABLE_DATA_STAT_PROBLEM);
		executable_data_path = NULL;
	}

	/*
	Finds the process file path of the executable.

	The location of the file is first guessed and
	the existence of the file is then checked.
	*/
	if (executable_data_path == NULL) {
		executable_process_path = NULL;
	}
	else {
		const size_t size = strlen(executable_data_path)+1+strlen(executable_process_file)+1;
		executable_process_path = malloc(size);
		snprintf(executable_process_path, size, "%s/%s", executable_data_path, executable_process_file);
		if (stat(executable_process_path, &buf) != 0) {
			//note(EXECUTABLE_PROCESS_STAT_PROBLEM);
			executable_process_path = NULL;
		}
	}

	/*
	Finds the version file path of the executable.

	The location of the file is first guessed and
	the existence of the file is then checked.
	*/
	if (executable_data_path == NULL) {
		warning(EXECUTABLE_VERSION_STAT_PROBLEM);
		executable_version_path = NULL;
	}
	else {
		const size_t size = strlen(executable_data_path)+1+strlen(executable_version_file)+1;
		executable_version_path = malloc(size);
		snprintf(executable_version_path, size, "%s/%s", executable_data_path, executable_version_file);
		if (stat(executable_version_path, &buf) != 0) {
			warning(EXECUTABLE_VERSION_STAT_PROBLEM);
			executable_version_path = NULL;
		}
	}

	/*
	Finds the count file path of the executable.

	The location of the file is first guessed and
	the existence of the file is then checked.
	*/
	if (executable_data_path == NULL) {
		error(EXECUTABLE_COUNT_STAT_PROBLEM);
		executable_count_path = NULL;
	}
	else {
		const size_t size = strlen(executable_data_path)+1+strlen(executable_count_file)+1;
		executable_count_path = malloc(size);
		snprintf(executable_count_path, size, "%s/%s", executable_data_path, executable_count_file);
	}

	executable_keybind_file = NULL;//TODO keybinding file

	executable_config_file = NULL;//TODO configuration file

	return NO_PROBLEM;
}

problem_t init_loader_config() {
	PROPAGATE(init_common_config());

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
	Finds the size of the terminal.

	The configuration file is first parsed and
	the default size is then guessed.
	*/
	if (config_lookup_int(&config, "rows", &rows) == 0) {
		warning(CONFIG_ROW_PROBLEM);
		rows = default_rows;
	}
	if (config_lookup_int(&config, "cols", &cols) == 0) {
		warning(CONFIG_COL_PROBLEM);
		cols = default_cols;
	}
	rows = MIN(MAX(executable_rows_min, rows), executable_rows_max);
	cols = MIN(MAX(executable_cols_min, cols), executable_cols_max);

	/*
	Finds the amount of save states.

	The configuration file is first parsed and
	the default size is then guessed.
	*/
	if (config_lookup_int(&config, "states", &states) == 0) {
		warning(CONFIG_STATE_PROBLEM);
		states = default_states;
	}
	if (states < 1) {
		warning(STATE_AMOUNT_PROBLEM);
		states = 1;
	}
	states++;//reserves space for the currently active state

	/*
	Finds the replacement string.

	The configuration file is first parsed and
	the default string is then guessed.
	*/
	char replacement;
	const char * replacement_string;
	if (config_lookup_string(&config, "iterator", &replacement_string) == 0) {
		warning(CONFIG_ITERATOR_PROBLEM);
		replacement = default_iterator;
	}
	else {
		replacement = replacement_string[0];
	}

	/*
	Opens the put streams.

	The configuration file is first parsed,
	the existence of the put file is then checked and
	the index character is then located,
	if the index character is not found
		the file is used only for the currently active save state,
	otherwise
		the index character is first replaced with the corresponding save state number and
		the file is then used for all save states.
	*/
	const char * input_path;
	if (config_lookup_string(&config, "input", &input_path) == 0) {
		warning(CONFIG_INPUT_PROBLEM);
		input_path = default_input_path;
	}
	if (stat(input_path, &buf) == 0) {
		warning(INPUT_FIND_PROBLEM);
	}
	else {
		input_stream = fopen(input_path, "rb");
		if (input_stream == NULL) {
			warning(INPUT_ACCESS_PROBLEM);
		}
	}
	const char * output_path;
	if (config_lookup_string(&config, "output", &output_path) == 0) {
		warning(CONFIG_OUTPUT_PROBLEM);
		output_path = default_output_path;
	}
	char ** output_files = malloc(states*sizeof *output_files);
	const char * output_path_end = strchr(output_path, replacement);
	if (strrchr(output_path, replacement) != output_path_end) {
		warning(OUTPUT_REPLACEMENT_PROBLEM);
	}
	if (output_path_end == NULL) {
		output_files[0] = malloc(strlen(output_path)+1);
		strcpy(output_files[0], output_path);
		for (unsigned int index = 1; index < states; index++) {
			output_files[index] = NULL;
		}
	}
	else {
		for (unsigned int index = 0; index < states; index++) {
			const size_t end_distance = output_path_end-output_path;
			const size_t length = intlen(index);
			const size_t size = strlen(output_path)-1+length+1;
			output_files[index] = malloc(size);
			strncpy(output_files[index], output_path, end_distance);
			snprintf(output_files[index]+end_distance, size-end_distance, "%u%s", index, output_path_end+1);
		}
	}
	output_streams = malloc(states*sizeof (*output_streams));
	for (unsigned int index = 0; index < states; index++) {
		if (output_files[index] != NULL) {
			if (stat(output_files[index], &buf) == 0) {
				warning(OUTPUT_OVERWRITE_PROBLEM);
			}
			else {
				output_streams[index] = fopen(output_files[index], "wb");
				if (output_streams[index] == NULL) {
					warning(OUTPUT_ACCESS_PROBLEM);
				}
			}
		}
	}

	/*
	Opens the log streams.

	The configuration file is first parsed,
	the existence of the log file is then checked,
	the log file is then created and
	the log stream is finally opened.
	*/
	FILE * new_error_stream;
	FILE * new_warning_stream;
	FILE * new_note_stream;
	FILE * new_call_stream;
	const char * error_path;
	if (config_lookup_string(&config, "errors", &error_path) == 0) {
		warning(CONFIG_ERROR_LOG_PROBLEM);
		STDSTR(new_error_stream, default_error_stream);
	}
	else {
		STDSTR(new_error_stream, error_path)//!
		else {
			if (stat(error_path, &buf) == 0) {
				note(ERROR_LOG_OVERWRITE_PROBLEM);
			}
			new_error_stream = fopen(error_path, "w");
			if (new_error_stream == NULL) {
				warning(ERROR_LOG_ACCESS_PROBLEM);
				STDSTR(new_error_stream, default_error_stream);
			}
		}
	}
	const char * warning_path;
	if (config_lookup_string(&config, "warnings", &warning_path) == 0) {
		warning(CONFIG_WARNING_LOG_PROBLEM);
		STDSTR(new_warning_stream, default_warning_stream);
	}
	else {
		STDSTR(new_warning_stream, warning_path)//!
		else {
			if (stat(warning_path, &buf) == 0) {
				note(WARNING_LOG_OVERWRITE_PROBLEM);
			}
			new_warning_stream = fopen(warning_path, "w");
			if (new_warning_stream == NULL) {
				warning(WARNING_LOG_ACCESS_PROBLEM);
				STDSTR(new_warning_stream, default_warning_stream);
			}
		}
	}
	const char * note_path;
	if (config_lookup_string(&config, "notes", &note_path) == 0) {
		warning(CONFIG_NOTE_LOG_PROBLEM);
		STDSTR(new_note_stream, default_note_stream);
	}
	else {
		STDSTR(new_note_stream, note_path)//!
		else {
			if (stat(note_path, &buf) == 0) {
				note(NOTE_LOG_OVERWRITE_PROBLEM);
			}
			new_note_stream = fopen(note_path, "w");
			if (new_note_stream == NULL) {
				warning(NOTE_LOG_ACCESS_PROBLEM);
				STDSTR(new_note_stream, default_note_stream);
			}
		}
	}
	const char * call_path;
	if (config_lookup_string(&config, "calls", &call_path) == 0) {
		warning(CONFIG_CALL_LOG_PROBLEM);
		STDSTR(new_call_stream, default_call_stream);
	}
	else {
		STDSTR(new_call_stream, call_path)//!
		else {
			if (stat(call_path, &buf) == 0) {
				note(CALL_LOG_OVERWRITE_PROBLEM);
			}
			new_call_stream = fopen(call_path, "w");
			if (new_call_stream == NULL) {
				warning(CALL_LOG_ACCESS_PROBLEM);
				STDSTR(new_call_stream, default_call_stream);
			}
		}
		if (new_call_stream != NULL) {
			note(CALL_LOG_PROBLEM);
		}
	}
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

	/*
	Finds the location of the shared memory segment.

	The configuration file is first parsed and
	the default location is then guessed.
	*/
	const char * shm_slurp;
	if (config_lookup_string(&config, "shm", &shm_slurp) == 0) {
		warning(CONFIG_SHM_PROBLEM);
		shm_path = malloc(strlen(default_shm_path)+1);
		strcpy(shm_path, default_shm_path);
	}
	else {
		shm_path = malloc(strlen(shm_slurp)+1);
		strcpy(shm_path, shm_path);
	}

	/*
	Unloads the configuration file.

	The memory allocated by the <code>config_lookup_</code> calls is automatically deallocated.
	*/
	config_destroy(&config);

	return NO_PROBLEM;
}

#endif
