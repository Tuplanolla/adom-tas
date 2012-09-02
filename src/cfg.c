/**
Manages the configuration variables.

Some redundant conditions are used to
 enforce predictable behavior and
 make maintenance (return point changes) easier.

TODO check output_paths management

@file cfg.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#include <stdlib.h>//*alloc, free, *env, size_t, NULL
#include <stdio.h>//*open, *close, *read, *write, *print*, FILE, EOF
#include <string.h>//str*
#include <sys/stat.h>//stat, S_*
#include <pwd.h>//*id, *wd

#include <libconfig.h>//config_*, CONFIG_*

#include "util.h"//hash, stdstr, SUBNULL
#include "prob.h"//probno, *_PROBLEM
#include "log.h"//log_*
#include "def.h"//def_*
#include "exec.h"//exec_*
#include "rec.h"//record

#include "cfg.h"

/**
The location of the home directory.
**/
char * cfg_home_path;
/**
The location of the executable.
**/
char * cfg_exec_path;
/**
The location of the data directory of the executable.
**/
char * cfg_exec_data_path;
/**
The location of the temporary file directory of the executable.
**/
char * cfg_exec_temporary_path;
/**
The locations of the temporary files of the executable.
**/
char ** cfg_exec_temporary_paths;
/**
The location of the configuration file of the executable.
**/
char * cfg_exec_config_path;
/**
The location of the process lock file of the executable.
**/
char * cfg_exec_process_path;
/**
The location of the keybinding file of the executable.
**/
char * cfg_exec_keybind_path;
/**
The location of the version file of the executable.
**/
char * cfg_exec_version_path;
/**
The location of the error report file of the executable.
**/
char * cfg_exec_error_path;
/**
The location of the character count file of the executable.
**/
char * cfg_exec_count_path;
/**
The location of this library.
**/
char * cfg_lib_path;
/**
The location of the C standard library.
**/
char * cfg_libc_path;
/**
The location of the New Cursor Optimization library.
**/
char * cfg_libncurses_path;
/**
Whether default configurations are enforced.
**/
bool cfg_force_sync;
/**
The amount of save states.
**/
int cfg_saves;
/**
The height of the terminal.
**/
int cfg_rows;
/**
The width of the terminal.
**/
int cfg_cols;
/**
The location of the shared memory segment.
**/
char * cfg_shm_path;
/**
The initial system time.
**/
time_t cfg_timestamp;
/**
The amount of generated characters.
**/
unsigned int cfg_generations;
/**
Whether save-quit-load emulation is enabled.
**/
bool cfg_emulate_sql;
/**
Whether save states are preserved upon loading them.
**/
bool cfg_keep_saves;
/**
Whether the input file is played back automatically.
**/
bool cfg_play_instantly;
/**
Whether the graphical user interface is colorful.
**/
bool cfg_monochrome;
/**
The string to replace with a number.
**/
char * cfg_iterator;
/**
The location of the input file.
**/
char * cfg_input_path;
/**
The locations of the output files.
**/
char ** cfg_output_paths;
/**
The error stream.
**/
FILE * cfg_error_stream;
/**
The warning stream.
**/
FILE * cfg_warning_stream;
/**
The notice stream.
**/
FILE * cfg_notice_stream;
/**
The call stream.
**/
FILE * cfg_call_stream;
/**
The key used to save the active state.
**/
int cfg_save_key;
/**
The key used to load the active state.
**/
int cfg_load_key;
/**
The key used to change the active state by incrementing.
**/
int cfg_next_save_key;
/**
The key used to change the active state by decrementing.
**/
int cfg_prev_save_key;
/**
The key used to change the frame duration by incrementing.
**/
int cfg_longer_duration_key;
/**
The key used to change the frame duration by decrementing.
**/
int cfg_shorter_duration_key;
/**
The key used to change the system time by incrementing.
**/
int cfg_more_time_key;
/**
The key used to change the system time by decrementing.
**/
int cfg_less_time_key;
/**
The key used to open or close the save state menu.
**/
int cfg_menu_key;
/**
The key used to open or close the information screen.
**/
int cfg_info_key;
/**
The key used to condense or expand the graphical user interface.
**/
int cfg_condense_key;
/**
The key used to hide or show the graphical user interface.
**/
int cfg_hide_key;
/**
The key used to play back a record.
**/
int cfg_play_key;
/**
The key used to stop a record to create a branch.
**/
int cfg_stop_key;
/**
The key used to terminate all processes.
**/
int cfg_quit_key;

/**
The configuration.
**/
static config_t config;

/**
Uninitializes the configuration variables.

@return The error code.
**/
int cfg_uninit(void) {
	int result = 0;
	free(cfg_home_path);
	free(cfg_exec_path);
	free(cfg_exec_data_path);
	free(cfg_exec_temporary_path);
	if (cfg_exec_temporary_paths != NULL) {
		for (unsigned int level = 0; level < exec_temporary_levels; level++) {
			const unsigned int offset = level * exec_temporary_parts;
			for (unsigned int part = 0; part < exec_temporary_parts; part++) {
				const unsigned int path = offset + part;
				free(cfg_exec_temporary_paths[path]);
			}
		}
		free(cfg_exec_temporary_paths);
	}
	free(cfg_exec_config_path);
	free(cfg_exec_process_path);
	free(cfg_exec_keybind_path);
	free(cfg_exec_version_path);
	free(cfg_exec_error_path);
	free(cfg_exec_count_path);
	free(cfg_lib_path);
	free(cfg_libc_path);
	free(cfg_libncurses_path);
	free(cfg_shm_path);
	free(cfg_iterator);
	free(cfg_input_path);
	if (cfg_output_paths != NULL) {
		for (int state = 1; state < cfg_saves; state++) {
			free(cfg_output_paths[state]);
		}
		free(cfg_output_paths);
	}
	if (cfg_error_stream != NULL) {
		if (fclose(cfg_error_stream) == EOF) {
			probno = log_error(ERROR_CLOSE_PROBLEM);
			result = -1;
		}
		else {
			if (cfg_warning_stream == cfg_error_stream) {
				cfg_warning_stream = NULL;
			}
			if (cfg_notice_stream == cfg_error_stream) {
				cfg_notice_stream = NULL;
			}
			if (cfg_call_stream == cfg_error_stream) {
				cfg_call_stream = NULL;
			}
		}
	}
	if (cfg_warning_stream != NULL) {
		if (fclose(cfg_warning_stream) == EOF) {
			probno = log_error(WARNING_CLOSE_PROBLEM);
			result = -1;
		}
		else {
			if (cfg_notice_stream == cfg_warning_stream) {
				cfg_notice_stream = NULL;
			}
			if (cfg_call_stream == cfg_warning_stream) {
				cfg_call_stream = NULL;
			}
		}
	}
	if (cfg_notice_stream != NULL) {
		if (fclose(cfg_notice_stream) == EOF) {
			probno = log_error(NOTE_CLOSE_PROBLEM);
			result = -1;
		}
		else {
			if (cfg_call_stream == cfg_notice_stream) {
				cfg_call_stream = NULL;
			}
		}
	}
	if (cfg_call_stream != NULL) {
		if (fclose(cfg_call_stream) == EOF) {
			probno = log_error(CALL_CLOSE_PROBLEM);
			result = -1;
		}
	}
	return result;
}

/**
Initializes the common configuration variables.

@return The error code.
**/
static int begin_init_config(void) {
	/*
	Sets the starting values that are used during initialization.
	*/
	cfg_home_path = NULL;
	cfg_exec_path = NULL;
	cfg_exec_data_path = NULL;
	cfg_exec_temporary_path = NULL;
	cfg_exec_temporary_paths = NULL;
	cfg_exec_config_path = NULL;
	cfg_exec_process_path = NULL;
	cfg_exec_keybind_path = NULL;
	cfg_exec_version_path = NULL;
	cfg_exec_error_path = NULL;
	cfg_exec_count_path = NULL;
	cfg_lib_path = NULL;
	cfg_libc_path = NULL;
	cfg_libncurses_path = NULL;
	cfg_shm_path = NULL;
	cfg_iterator = NULL;
	cfg_input_path = NULL;
	cfg_output_paths = NULL;
	cfg_error_stream = stdstr(def_error_stream);
	cfg_warning_stream = stdstr(def_warning_stream);
	cfg_notice_stream = stdstr(def_notice_stream);
	cfg_call_stream = stdstr(def_call_stream);


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
	if (stat(def_config_path, &config_stat) == -1) {
		FILE * stream = fopen(def_config_path, "w");
		if (stream == NULL) {
			probno = log_error(LIBCONFIG_OPEN_PROBLEM);
			return -1;
		}
		else {
			const size_t size = strlen(def_config) + 1;
			if (fwrite(def_config, size, 1, stream) != 1) {
				probno = log_error(LIBCONFIG_WRITE_PROBLEM);
			}
			if (fclose(stream) == EOF) {
				probno = log_error(LIBCONFIG_CLOSE_PROBLEM);
			}
		}
		probno = log_notice(LIBCONFIG_STAT_PROBLEM);
		return -1;
	}
	config_init(&config);
	if (config_read_file(&config, def_config_path) == 0) {
		probno = log_error(LIBCONFIG_PARSE_PROBLEM);
		return -1;
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
		//probno = log_warning(HOME_CONFIG_PROBLEM);
		new_home_path = getenv("HOME");
		if (new_home_path == NULL) {
			//probno = log_warning(HOME_GETENV_PROBLEM);
			pw = getpwuid(getuid());
			if (pw == NULL) {
				new_home_path = NULL;
				probno = log_warning(HOME_GETPWUID_PROBLEM);
			}
			else {
				new_home_path = pw->pw_dir;
			}
		}
	}
	if (new_home_path == NULL) {
		cfg_home_path = NULL;
	}
	else {
		cfg_home_path = malloc(strlen(new_home_path) + 1);
		if (cfg_home_path == NULL) {
			probno = log_error(MALLOC_PROBLEM);
			return -1;
		}
		else {
			strcpy(cfg_home_path, new_home_path);
			struct stat home_stat;
			if (stat(cfg_home_path, &home_stat) == -1) {
				free(cfg_home_path);
				cfg_home_path = NULL;
				probno = log_warning(HOME_STAT_PROBLEM);
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
		//probno = log_warning(EXEC_DATA_CONFIG_PROBLEM);
		if (cfg_home_path == NULL) {
			cfg_exec_data_path = NULL;
		}
		else {
			const size_t size = strlen(cfg_home_path) + 1
					+ strlen(exec_data_directory) + 1;
			cfg_exec_data_path = malloc(size);
			if (cfg_exec_data_path == NULL) {
				probno = log_error(MALLOC_PROBLEM);
				return -1;
			}
			else {
				snprintf(cfg_exec_data_path, size, "%s/%s",
						cfg_home_path,
						exec_data_directory);
			}
		}
	}
	else {
		cfg_exec_data_path = astrrep(new_exec_data_path, "~", cfg_home_path);
	}
	if (cfg_exec_data_path == NULL) {
		probno = log_error(NULL_PROBLEM);
		return -1;
	}
	else {
		struct stat exec_data_stat;
		if (stat(cfg_exec_data_path, &exec_data_stat) == -1) {
			probno = log_warning(EXEC_DATA_STAT_PROBLEM);
		}
	}

	/*
	Finds the temporary file directory path of the executable.

	The location of the directory is first guessed and
	 the existence of the directory is then checked.
	*/
	if (cfg_exec_data_path == NULL) {
		cfg_exec_temporary_path = NULL;
	}
	else {
		const size_t size = strlen(cfg_exec_data_path) + 1
				+ strlen(exec_temporary_directory) + 1;
		cfg_exec_temporary_path = malloc(size);
		if (cfg_exec_temporary_path == NULL) {
			probno = log_error(MALLOC_PROBLEM);
			return -1;
		}
		else {
			snprintf(cfg_exec_temporary_path, size, "%s/%s",
					cfg_exec_data_path,
					exec_temporary_directory);
			struct stat exec_temporary_stat;
			if (stat(cfg_exec_temporary_path, &exec_temporary_stat) == -1) {
				probno = log_warning(EXEC_TEMPORARY_STAT_PROBLEM);
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
	if (cfg_exec_temporary_path == NULL) {
		cfg_exec_temporary_paths = NULL;
	}
	else {
		cfg_exec_temporary_paths = malloc(exec_temporary_levels * exec_temporary_parts * sizeof *cfg_exec_temporary_paths);
		if (cfg_exec_temporary_paths == NULL) {
			probno = log_error(MALLOC_PROBLEM);
			return -1;
		}
		else {
			const int level_width = uintlen(exec_temporary_levels);
			const int part_width = uintlen(exec_temporary_parts);
			for (unsigned int level = 0; level < exec_temporary_levels; level++) {
				const unsigned int offset = level * exec_temporary_parts;
				for (unsigned int part = 0; part < exec_temporary_parts; part++) {
					const unsigned int path = offset + part;
					const size_t size = strlen(cfg_exec_temporary_path) + 1
							+ strlen(exec_temporary_file)
							+ 4 + 1;
					cfg_exec_temporary_paths[path] = malloc(size);
					if (cfg_exec_temporary_paths[path] == NULL) {
						probno = log_error(MALLOC_PROBLEM);
						return -1;
					}
					else {
						snprintf(cfg_exec_temporary_paths[path], size, "%s/%s%0*u_%0*u",
								cfg_exec_temporary_path,
								exec_temporary_file, level_width, level, part_width, part);
					}
				}
			}
		}
	}

	return 0;
}

/**
Finishes initializing the configuration.

@return The error code.
**/
static int end_init_config(void) {
	/*
	Closes the configuration file.

	The memory allocated by the <code>config_lookup_*</code> calls is
	 automatically deallocated.
	*/
	config_destroy(&config);

	return 0;
}

int cfg_init_main(void) {
	if (begin_init_config() == -1) {
		return -1;
	}

	/*
	Enables preloading libraries.

	The configuration file is first searched,
	 the environment variable is then gotten,
	 the existence of the file is then checked and
	 the environment variable is finally set.
	*/
	const char * new_lib_path;
	if (config_lookup_string(&config, "lib", &new_lib_path) == CONFIG_FALSE) {
		//probno = log_warning(LD_PRELOAD_CONFIG_PROBLEM);
		new_lib_path = getenv("LD_PRELOAD");
		if (cfg_lib_path == NULL) {
			new_lib_path = def_lib_path;
			probno = log_warning(LD_PRELOAD_GETENV_PROBLEM);
		}
	}
	if (new_lib_path == NULL) {
		cfg_lib_path = NULL;
	}
	else {
		cfg_lib_path = astrrep(new_lib_path, "~", cfg_home_path);
		struct stat lib_stat;
		if (stat(cfg_lib_path, &lib_stat) == -1) {
			free(cfg_lib_path);
			cfg_lib_path = NULL;
			probno = log_error(LD_PRELOAD_STAT_PROBLEM);
			return -1;
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
		new_exec_path = def_exec_path;
		//probno = log_warning(EXEC_CONFIG_PROBLEM);
	}
	if (new_exec_path == NULL) {
		cfg_exec_path = NULL;
	}
	else {
		cfg_exec_path = astrrep(new_exec_path, "~", cfg_home_path);
		struct stat exec_stat;
		if (stat(cfg_exec_path, &exec_stat) == -1) {
			free(cfg_exec_path);
			cfg_exec_path = NULL;
			probno = log_error(EXEC_STAT_PROBLEM);
			return -1;
		}
		else if (S_ISDIR(exec_stat.st_mode)) {
			free(cfg_exec_path);
			cfg_exec_path = NULL;
			probno = log_error(EXEC_TYPE_PROBLEM);
			return -1;
		}
		else if ((exec_stat.st_mode & (S_ISUID | S_ISGID)) != 0) {
			free(cfg_exec_path);
			cfg_exec_path = NULL;
			probno = log_error(EXEC_PERMISSION_PROBLEM);
			return -1;
		}
		else {
			const size_t size = (size_t )exec_stat.st_size;
			if (size != exec_size) {
				probno = log_warning(EXEC_SIZE_PROBLEM);
			}
			else {
				FILE * stream = fopen(cfg_exec_path, "rb");
				if (stream == NULL) {
					probno = log_error(EXEC_OPEN_PROBLEM);
				}
				else {
					unsigned char * ptr = malloc(size);
					if (ptr == NULL) {
						probno = log_error(MALLOC_PROBLEM);
						return -1;
					}
					else {
						if (fread(ptr, 1, size, stream) != exec_size) {
							probno = log_error(EXEC_READ_PROBLEM);
						}
						else if (hash(ptr, exec_size) != exec_hash) {
							probno = log_warning(EXEC_HASH_PROBLEM);
						}
						free(ptr);
						if (fclose(stream) == EOF) {
							probno = log_error(EXEC_CLOSE_PROBLEM);
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
	if (cfg_exec_data_path == NULL) {
		cfg_exec_config_path = NULL;
	}
	else {
		const size_t size = strlen(cfg_exec_data_path) + 1
				+ strlen(exec_config_file) + 1;
		cfg_exec_config_path = malloc(size);
		if (cfg_exec_config_path == NULL) {
			probno = log_error(MALLOC_PROBLEM);
			return -1;
		}
		else {
			snprintf(cfg_exec_config_path, size, "%s/%s",
					cfg_exec_data_path,
					exec_config_file);
			struct stat exec_config_stat;
			if (stat(cfg_exec_config_path, &exec_config_stat) == -1) {
				probno = log_warning(EXEC_CONFIG_STAT_PROBLEM);
			}
		}
	}

	/*
	Finds the process lock file path of the executable.

	The location of the file is first guessed and
	 the existence of the file is then checked.
	*/
	if (cfg_exec_data_path == NULL) {
		cfg_exec_process_path = NULL;
	}
	else {
		const size_t size = strlen(cfg_exec_data_path) + 1
				+ strlen(exec_process_file) + 1;
		cfg_exec_process_path = malloc(size);
		if (cfg_exec_process_path == NULL) {
			probno = log_error(MALLOC_PROBLEM);
			return -1;
		}
		else {
			snprintf(cfg_exec_process_path, size, "%s/%s",
					cfg_exec_data_path,
					exec_process_file);
			struct stat exec_process_stat;
			if (stat(cfg_exec_process_path, &exec_process_stat) == -1) {
				//probno = log_notice(EXEC_PROCESS_STAT_PROBLEM);
			}
		}
	}

	/*
	Finds the keybind file path of the executable.

	The location of the file is first guessed and
	 the existence of the file is then checked.
	*/
	if (cfg_exec_data_path == NULL) {
		cfg_exec_keybind_path = NULL;
	}
	else {
		const size_t size = strlen(cfg_exec_data_path) + 1
				+ strlen(exec_keybind_file) + 1;
		cfg_exec_keybind_path = malloc(size);
		if (cfg_exec_keybind_path == NULL) {
			probno = log_error(MALLOC_PROBLEM);
			return -1;
		}
		else {
			snprintf(cfg_exec_keybind_path, size, "%s/%s",
					cfg_exec_data_path,
					exec_keybind_file);
			struct stat exec_keybind_stat;
			if (stat(cfg_exec_keybind_path, &exec_keybind_stat) == -1) {
				probno = log_warning(EXEC_KEYBIND_STAT_PROBLEM);
			}
		}
	}

	/*
	Finds the version file path of the executable.

	The location of the file is first guessed and
	 the existence of the file is then checked.
	*/
	if (cfg_exec_data_path == NULL) {
		cfg_exec_version_path = NULL;
	}
	else {
		const size_t size = strlen(cfg_exec_data_path) + 1
				+ strlen(exec_version_file) + 1;
		cfg_exec_version_path = malloc(size);
		if (cfg_exec_version_path == NULL) {
			probno = log_error(MALLOC_PROBLEM);
			return -1;
		}
		else {
			snprintf(cfg_exec_version_path, size, "%s/%s",
					cfg_exec_data_path,
					exec_version_file);
			struct stat exec_version_stat;
			if (stat(cfg_exec_version_path, &exec_version_stat) == -1) {
				probno = log_warning(EXEC_VERSION_STAT_PROBLEM);
			}
		}
	}

	/*
	Finds the error log file path of the executable.

	The location of the file is first guessed and
	 the existence of the file is then checked.
	*/
	if (cfg_exec_data_path == NULL) {
		cfg_exec_error_path = NULL;
	}
	else {
		const size_t size = strlen(cfg_exec_data_path) + 1
				+ strlen(exec_error_file) + 1;
		cfg_exec_error_path = malloc(size);
		if (cfg_exec_error_path == NULL) {
			probno = log_error(MALLOC_PROBLEM);
			return -1;
		}
		else {
			snprintf(cfg_exec_error_path, size, "%s/%s",
					cfg_exec_data_path,
					exec_error_file);
			struct stat exec_error_stat;
			if (stat(cfg_exec_error_path, &exec_error_stat) == -1) {
				//probno = log_notice(EXEC_ERROR_STAT_PROBLEM);
			}
		}
	}

	/*
	Finds the count file path of the executable.

	The location of the file is first guessed and
	 the existence of the file is then checked.
	*/
	if (cfg_exec_data_path == NULL) {
		cfg_exec_count_path = NULL;
	}
	else {
		const size_t size = strlen(cfg_exec_data_path) + 1
				+ strlen(exec_count_file) + 1;
		cfg_exec_count_path = malloc(size);
		if (cfg_exec_count_path == NULL) {
			probno = log_error(MALLOC_PROBLEM);
			return -1;
		}
		else {
			snprintf(cfg_exec_count_path, size, "%s/%s",
					cfg_exec_data_path,
					exec_count_file);
			struct stat exec_count_stat;
			if (stat(cfg_exec_count_path, &exec_count_stat) == -1) {
				probno = log_warning(EXEC_COUNT_STAT_PROBLEM);
			}
		}
	}

	if (end_init_config() == -1) {
		return -1;
	}

	return 0;
}

int cfg_init_lib(void) {
	if (begin_init_config() == -1) {
		return -1;
	}

	/*
	Finds the libraries to load.

	The configuration file is first parsed,
	 the environment variable is then read and
	 the existence of the file is then checked.
	*/
	const char * new_libc_path;
	if (config_lookup_string(&config, "libc", &new_libc_path) == CONFIG_FALSE) {
		new_libc_path = def_libc_path;
		probno = log_warning(LIBC_CONFIG_PROBLEM);
	}
	cfg_libc_path = astrrep(new_libc_path, "~", cfg_home_path);
	struct stat libc_stat;
	if (stat(cfg_libc_path, &libc_stat) == -1) {
		free(cfg_libc_path);
		cfg_libc_path = NULL;
		probno = log_error(LIBC_STAT_PROBLEM);
		return -1;
	}

	const char * new_libncurses_path;
	if (config_lookup_string(&config, "libncurses", &new_libncurses_path) == CONFIG_FALSE) {
		new_libncurses_path = def_libncurses_path;
		probno = log_warning(LIBNCURSES_CONFIG_PROBLEM);
	}
	cfg_libncurses_path = astrrep(new_libncurses_path, "~", cfg_home_path);
	struct stat libncurses_stat;
	if (stat(cfg_libncurses_path, &libncurses_stat) != 0) {
		free(cfg_libncurses_path);
		cfg_libncurses_path = NULL;
		probno = log_error(LIBNCURSES_STAT_PROBLEM);
		return -1;
	}

	/**
	Sets the save state perseverance.
	**/
	int new_force_sync;
	if (config_lookup_bool(&config, "force_sync", &new_force_sync) == CONFIG_FALSE) {
		new_force_sync = def_force_sync;
		//probno = log_warning(FORCE_SYNC_CONFIG_PROBLEM);
	}
	cfg_force_sync = new_force_sync == CONFIG_TRUE;

	/*
	Finds the amount of save states.

	The configuration file is first parsed,
	 the default amount is then assumed and
	 the bounds are finally checked.
	*/
	int new_saves;
	if (config_lookup_int(&config, "saves", &new_saves) == CONFIG_FALSE) {
		new_saves = def_saves;
		//probno = log_warning(STATE_CONFIG_PROBLEM);
	}
	if (new_saves < 1) {
		new_saves = 1;
		probno = log_warning(STATE_AMOUNT_PROBLEM);
	}
	cfg_saves = new_saves + 1;//reserves space for the active state

	/*
	Finds the height of the terminal.

	The configuration file is first parsed,
	 the default height is then assumed and
	 the bounds are finally checked.
	*/
	int new_rows;
	if (config_lookup_int(&config, "rows", &new_rows) == CONFIG_FALSE) {
		new_rows = def_rows;
		//probno = log_warning(ROW_CONFIG_PROBLEM);
	}
	if (new_rows < exec_rows_min || new_rows > exec_rows_max) {
		new_rows = MIN(MAX(exec_rows_min, new_rows), exec_rows_max);
		probno = log_warning(ROW_AMOUNT_PROBLEM);
	}
	cfg_rows = new_rows;

	/*
	Finds the width of the terminal.

	The configuration file is first parsed,
	 the default width is then assumed and
	 the bounds are finally checked.
	*/
	int new_cols;
	if (config_lookup_int(&config, "cols", &new_cols) == CONFIG_FALSE) {
		new_cols = def_cols;
		//probno = log_warning(COL_CONFIG_PROBLEM);
	}
	if (new_cols < exec_cols_min || new_cols > exec_cols_max) {
		new_cols = MIN(MAX(exec_cols_min, new_cols), exec_cols_max);
		probno = log_warning(COL_AMOUNT_PROBLEM);
	}
	cfg_cols = new_cols;

	/*
	Finds the location of the shared memory segment.

	The configuration file is first parsed and
	 the default location is then guessed.
	*/
	const char * new_shm_path;
	if (config_lookup_string(&config, "shm", &new_shm_path) == CONFIG_FALSE) {
		new_shm_path = def_config_path;//def_shm_path;
		//probno = log_warning(SHM_CONFIG_PROBLEM);
	}
	cfg_shm_path = astrrep(new_shm_path, "~", cfg_home_path);
	struct stat shm_stat;
	if (stat(cfg_shm_path, &shm_stat) == -1) {
		free(cfg_shm_path);
		cfg_shm_path = NULL;
		probno = log_error(SHM_STAT_PROBLEM);
		return -1;
	}

	/**
	Sets the initial system time.
	**/
	int new_timestamp;
	if (config_lookup_int(&config, "timestamp", &new_timestamp) == CONFIG_FALSE) {
		new_timestamp = def_timestamp;
		//probno = log_warning(TIMESTAMP_CONFIG_PROBLEM);
	}
	cfg_timestamp = (time_t )new_timestamp;

	/**
	Sets the amount of generated characters.
	**/
	int new_generations;
	if (config_lookup_int(&config, "generations", &new_generations) == CONFIG_FALSE) {
		new_generations = def_generations;
		//probno = log_warning(GENERATIONS_CONFIG_PROBLEM);
	}
	cfg_generations = (unsigned int )new_generations;

	/**
	Sets the save-quit-load emulation.
	**/
	int new_emulate_sql;
	if (config_lookup_bool(&config, "emulate_sql", &new_emulate_sql) == CONFIG_FALSE) {
		new_emulate_sql = def_emulate_sql;
		//probno = log_warning(EMULATE_SQL_CONFIG_PROBLEM);
	}
	cfg_emulate_sql = new_emulate_sql == CONFIG_TRUE;

	/**
	Sets the save state perseverance.
	**/
	int new_keep_saves;
	if (config_lookup_bool(&config, "keep_saves", &new_keep_saves) == CONFIG_FALSE) {
		new_keep_saves = def_keep_saves;
		//probno = log_warning(KEEP_SAVES_CONFIG_PROBLEM);
	}
	cfg_keep_saves = new_keep_saves == CONFIG_TRUE;

	/**
	Sets the playback mode.
	**/
	int new_play_instantly;
	if (config_lookup_bool(&config, "play_instantly", &new_play_instantly) == CONFIG_FALSE) {
		new_play_instantly = def_play_instantly;
		//probno = log_warning(PLAY_INSTANTLY_CONFIG_PROBLEM);
	}
	cfg_play_instantly = new_play_instantly == CONFIG_TRUE;

	/**
	Sets the color mode.
	**/
	int new_monochrome;
	if (config_lookup_bool(&config, "monochrome", &new_monochrome) == CONFIG_FALSE) {
		new_monochrome = def_monochrome;
		//probno = log_warning(MONOCHROME_CONFIG_PROBLEM);
	}
	cfg_monochrome = new_monochrome == CONFIG_TRUE;

	/*
	Finds the iterator string.

	The configuration file is first parsed and
	 the default string is then assumed.
	*/
	const char * new_iterator;
	if (config_lookup_string(&config, "iterator", &new_iterator) == CONFIG_FALSE) {
		new_iterator = def_iterator;
		//probno = log_warning(ITERATOR_CONFIG_PROBLEM);
	}
	cfg_iterator = malloc(strlen(new_iterator) + 1);
	strcpy(cfg_iterator, new_iterator);

	/*
	Opens the input stream.

	The configuration file is first parsed and
	 the existence of the input file is then checked.
	*/
	const char * new_input_path;
	if (config_lookup_string(&config, "input", &new_input_path) == CONFIG_FALSE) {
		new_input_path = def_input_path;
		probno = log_warning(INPUT_CONFIG_PROBLEM);
	}
	cfg_input_path = astrrep(new_input_path, "~", cfg_home_path);
	struct stat input_stat;
	if (stat(cfg_input_path, &input_stat) == -1) {
		free(cfg_input_path);
		cfg_input_path = NULL;
		probno = log_warning(INPUT_STAT_PROBLEM);
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
		new_output_path = def_output_path;
		probno = log_warning(OUTPUT_CONFIG_PROBLEM);
	}
	char * const output_path = astrrep(new_output_path, "~", cfg_home_path);
	cfg_output_paths = malloc((size_t )cfg_saves * sizeof *cfg_output_paths);
	if (cfg_output_paths == NULL) {
		probno = log_error(MALLOC_PROBLEM);
		return -1;
	}
	else {
		bool exists = FALSE;
		for (int state = 1; state < cfg_saves; state++) {
			const size_t size = intlen(state) + 1;
			char * const iterand = malloc(size);
			if (iterand == NULL) {
				probno = log_error(MALLOC_PROBLEM);
				return -1;
			}
			else {
				snprintf(iterand, size, "%d", state);
				cfg_output_paths[state] = astrrep(output_path, cfg_iterator, iterand);
				free(iterand);
				struct stat output_stat;
				if (stat(cfg_output_paths[state], &output_stat) == 0) {
					exists = TRUE;
				}
			}
		}
		if (exists) {
			probno = log_warning(OUTPUT_STAT_PROBLEM);
		}
	}

	/*
	Opens the log streams.

	The configuration file is first parsed,
	 the existence of the log file is then checked,
	 the uniqueness is then checked,
	 the log file is then created and
	 the log stream is finally opened.
	*/
	const char * new_error_path;
	if (config_lookup_string(&config, "error_log", &new_error_path) == CONFIG_FALSE) {
		new_error_path = def_error_stream;
		probno = log_warning(ERROR_CONFIG_PROBLEM);
	}
	char * const error_path = astrrep(new_error_path, "~", cfg_home_path);
	FILE * new_error_stream = stdstr(error_path);
	struct stat error_stat;
	if (new_error_stream == NULL) {
		if (stat(error_path, &error_stat) == 0) {
			//probno = log_notice(ERROR_STAT_PROBLEM);
		}
		new_error_stream = fopen(error_path, "w");
		if (new_error_stream == NULL) {
			new_error_stream = NULL;
			probno = log_error(ERROR_OPEN_PROBLEM);
		}
	}
	free(error_path);

	const char * new_warning_path;
	if (config_lookup_string(&config, "warning_log", &new_warning_path) == CONFIG_FALSE) {
		new_warning_path = def_warning_stream;
		probno = log_warning(ERROR_CONFIG_PROBLEM);
	}
	char * const warning_path = astrrep(new_warning_path, "~", cfg_home_path);
	FILE * new_warning_stream = stdstr(warning_path);
	struct stat warning_stat;
	if (new_warning_stream == NULL) {
		if (stat(warning_path, &warning_stat) == 0) {
			//probno = log_notice(WARNING_STAT_PROBLEM);
		}
		if (warning_stat.st_dev == error_stat.st_dev
				&& warning_stat.st_ino == error_stat.st_ino) {
			new_warning_stream = new_error_stream;
		}
		else {
			new_warning_stream = fopen(warning_path, "w");
			if (new_warning_stream == NULL) {
				new_warning_stream = NULL;
				probno = log_error(WARNING_OPEN_PROBLEM);
			}
		}
	}
	free(warning_path);

	const char * new_notice_path;
	if (config_lookup_string(&config, "notice_log", &new_notice_path) == CONFIG_FALSE) {
		new_notice_path = def_notice_stream;
		probno = log_warning(ERROR_CONFIG_PROBLEM);
	}
	char * const notice_path = astrrep(new_notice_path, "~", cfg_home_path);
	FILE * new_notice_stream = stdstr(notice_path);
	struct stat notice_stat;
	if (new_notice_stream == NULL) {
		if (stat(notice_path, &notice_stat) == 0) {
			//probno = log_notice(NOTE_STAT_PROBLEM);
		}
		if (notice_stat.st_dev == error_stat.st_dev
				&& notice_stat.st_ino == error_stat.st_ino) {
			new_notice_stream = new_error_stream;
		}
		else if (notice_stat.st_dev == warning_stat.st_dev
				&& notice_stat.st_ino == warning_stat.st_ino) {
			new_notice_stream = new_warning_stream;
		}
		else {
			new_notice_stream = fopen(notice_path, "w");
			if (new_notice_stream == NULL) {
				new_notice_stream = NULL;
				probno = log_error(NOTE_OPEN_PROBLEM);
			}
		}
	}
	free(notice_path);

	const char * new_call_path;
	if (config_lookup_string(&config, "call_log", &new_call_path) == CONFIG_FALSE) {
		new_call_path = def_call_stream;
		probno = log_warning(ERROR_CONFIG_PROBLEM);
	}
	char * const call_path = astrrep(new_call_path, "~", cfg_home_path);
	FILE * new_call_stream = stdstr(call_path);
	struct stat call_stat;
	if (new_call_stream == NULL) {
		if (stat(call_path, &call_stat) == 0) {
			//probno = log_notice(CALL_STAT_PROBLEM);
		}
		if (call_stat.st_dev == error_stat.st_dev
				&& call_stat.st_ino == error_stat.st_ino) {
			new_call_stream = new_error_stream;
		}
		else if (call_stat.st_dev == warning_stat.st_dev
				&& call_stat.st_ino == warning_stat.st_ino) {
			new_call_stream = new_warning_stream;
		}
		else if (call_stat.st_dev == notice_stat.st_dev
				&& call_stat.st_ino == notice_stat.st_ino) {
			new_call_stream = new_notice_stream;
		}
		else {
			new_call_stream = fopen(call_path, "w");
			if (new_call_stream == NULL) {
				new_call_stream = NULL;
				probno = log_error(CALL_OPEN_PROBLEM);
			}
		}
	}
	free(call_path);

	if (new_error_stream != cfg_error_stream
			|| new_warning_stream != cfg_warning_stream
			|| new_notice_stream != cfg_notice_stream
			|| new_call_stream != cfg_call_stream) {
		probno = log_notice(LOG_CHANGE_PROBLEM);
		cfg_error_stream = new_error_stream;
		cfg_warning_stream = new_warning_stream;
		cfg_notice_stream = new_notice_stream;
		cfg_call_stream = new_call_stream;
	}

	/*
	Sets the keys.
	*/
	int new_save_key;
	if (config_lookup_int(&config, "save_key", &new_save_key) == CONFIG_FALSE) {
		new_save_key = def_save_key;
		//probno = log_warning(KEY_CONFIG_PROBLEM);
	}
	cfg_save_key = new_save_key;

	int new_load_key;
	if (config_lookup_int(&config, "load_key", &new_load_key) == CONFIG_FALSE) {
		new_load_key = def_load_key;
		//probno = log_warning(KEY_CONFIG_PROBLEM);
	}
	cfg_load_key = new_load_key;

	int new_next_state_key;
	if (config_lookup_int(&config, "next_state_key", &new_next_state_key) == CONFIG_FALSE) {
		new_next_state_key = def_next_save_key;
		//probno = log_warning(KEY_CONFIG_PROBLEM);
	}
	cfg_next_save_key = new_next_state_key;

	int new_prev_state_key;
	if (config_lookup_int(&config, "prev_state_key", &new_prev_state_key) == CONFIG_FALSE) {
		new_prev_state_key = def_prev_save_key;
		//probno = log_warning(KEY_CONFIG_PROBLEM);
	}
	cfg_prev_save_key = new_prev_state_key;

	int new_longer_duration_key;
	if (config_lookup_int(&config, "longer_duration_key", &new_longer_duration_key) == CONFIG_FALSE) {
		new_longer_duration_key = def_longer_duration_key;
		//probno = log_warning(KEY_CONFIG_PROBLEM);
	}
	cfg_longer_duration_key = new_longer_duration_key;

	int new_shorter_duration_key;
	if (config_lookup_int(&config, "shorter_duration_key", &new_shorter_duration_key) == CONFIG_FALSE) {
		new_shorter_duration_key = def_shorter_duration_key;
		//probno = log_warning(KEY_CONFIG_PROBLEM);
	}
	cfg_shorter_duration_key = new_shorter_duration_key;

	int new_more_time_key;
	if (config_lookup_int(&config, "time_key", &new_more_time_key) == CONFIG_FALSE) {
		new_more_time_key = def_more_time_key;
		//probno = log_warning(KEY_CONFIG_PROBLEM);
	}
	cfg_more_time_key = new_more_time_key;

	int new_less_time_key;
	if (config_lookup_int(&config, "untime_key", &new_less_time_key) == CONFIG_FALSE) {
		new_less_time_key = def_less_time_key;
		//probno = log_warning(KEY_CONFIG_PROBLEM);
	}
	cfg_less_time_key = new_less_time_key;

	int new_menu_key;
	if (config_lookup_int(&config, "menu_key", &new_menu_key) == CONFIG_FALSE) {
		new_menu_key = def_menu_key;
		//probno = log_warning(KEY_CONFIG_PROBLEM);
	}
	cfg_menu_key = new_menu_key;

	int new_info_key;
	if (config_lookup_int(&config, "info_key", &new_info_key) == CONFIG_FALSE) {
		new_info_key = def_info_key;
		//probno = log_warning(KEY_CONFIG_PROBLEM);
	}
	cfg_info_key = new_info_key;

	int new_condense_key;
	if (config_lookup_int(&config, "condense_key", &new_condense_key) == CONFIG_FALSE) {
		new_condense_key = def_condense_key;
		//probno = log_warning(KEY_CONFIG_PROBLEM);
	}
	cfg_condense_key = new_condense_key;

	int new_hide_key;
	if (config_lookup_int(&config, "hide_key", &new_hide_key) == CONFIG_FALSE) {
		new_hide_key = def_hide_key;
		//probno = log_warning(KEY_CONFIG_PROBLEM);
	}
	cfg_hide_key = new_hide_key;

	int new_play_key;
	if (config_lookup_int(&config, "play_key", &new_play_key) == CONFIG_FALSE) {
		new_play_key = def_play_key;
		//probno = log_warning(KEY_CONFIG_PROBLEM);
	}
	cfg_play_key = new_play_key;

	int new_stop_key;
	if (config_lookup_int(&config, "stop_key", &new_stop_key) == CONFIG_FALSE) {
		new_stop_key = def_stop_key;
		//probno = log_warning(KEY_CONFIG_PROBLEM);
	}
	cfg_stop_key = new_stop_key;

	int new_quit_key;
	if (config_lookup_int(&config, "quit_key", &new_quit_key) == CONFIG_FALSE) {
		new_quit_key = def_quit_key;
		//probno = log_warning(KEY_CONFIG_PROBLEM);
	}
	cfg_quit_key = new_quit_key;

	/*
	Sets the record information.
	*/
	const char * new_author;
	if (config_lookup_string(&config, "author", &new_author) == CONFIG_FALSE) {
		new_author = "";
		probno = log_notice(AUTHOR_CONFIG_PROBLEM);
	}
	strncpy(record.author, new_author, sizeof record.author);
	record.author[sizeof record.author - 1] = '\0';

	const char * new_comments;
	if (config_lookup_string(&config, "comments", &new_comments) == CONFIG_FALSE) {
		new_comments = "";
		probno = log_notice(COMMENTS_CONFIG_PROBLEM);
	}
	strncpy(record.comments, new_comments, sizeof record.comments);
	record.comments[sizeof record.comments - 1] = '\0';

	int new_category;
	if (config_lookup_int(&config, "category", &new_category) == CONFIG_FALSE) {
		new_category = 0;
		probno = log_notice(CATEGORY_CONFIG_PROBLEM);
	}
	record.category = new_category;//TODO outsource

	if (end_init_config() == -1) {
		return -1;
	}

	return 0;
}
