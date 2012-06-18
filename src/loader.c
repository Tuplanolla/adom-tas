/**
Modifies the executable.
**/
#ifndef LOADER_C
#define LOADER_C

#include <stdlib.h>//TODO get rid of the unnecessary
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include <curses.h>
#include <libconfig.h>

#include "util.h"
#include "problem.h"
#include "log.h"
#include "config.h"
#include "put.h"
#include "asm.h"
#include "shm.h"
#include "adom.h"
#include "record.h"

int rows;
int cols;
int states;
char * shm_file;
FILE * input_stream;
FILE ** output_streams;
FILE * error_stream;
FILE * warning_stream;
FILE * note_stream;
FILE * call_stream;

/**
Uninitializes this process.

Contains unnecessary checks.
**/
void uninit_child(problem_t code) {
	shm_detach();

	/*
	Closes the log streams.
	*/
	if (error_stream != NULL) fclose(error_stream);
	if (warning_stream != NULL) fclose(warning_stream);
	if (note_stream != NULL) fclose(note_stream);
	if (call_stream != NULL) fclose(call_stream);

	/*
	Exits gracefully.
	*/
	exit(code);
}

/**
Uninitializes all processes.

Contains unnecessary checks.
**/
void uninit_parent(problem_t code) {
	shm_uninit();

	/*
	Closes the log streams.
	*/
	if (error_stream != NULL) fclose(error_stream);
	if (warning_stream != NULL) fclose(warning_stream);
	if (note_stream != NULL) fclose(note_stream);
	if (call_stream != NULL) fclose(call_stream);

	/*
	Exits gracefully.
	*/
	exit(code);
}

#undef STDSTR
#define STDSTR(stream, str) \
	if (strcmp(str, "null") == 0) stream = NULL;\
	else if (strcmp(str, "stdin") == 0) stream = stdin;\
	else if (strcmp(str, "stdout") == 0) stream = stdout;\
	else if (strcmp(str, "stderr") == 0) stream = stderr;

void init_config() {
	struct stat buf;

	/*
	Sets the streams to their default values.

	The default values are used until the initialization finishes.
	*/
	STDSTR(error_stream, default_error_name);
	STDSTR(warning_stream, default_warning_name);
	STDSTR(note_stream, default_note_name);
	STDSTR(call_stream, default_call_name);

	/*
	Loads the configuration file.
	*/
	config_t config;
	config_init(&config);
	if (config_read_file(&config, default_config_name) == 0) {
		config_destroy(&config);
		uninit(error(CONFIG_PROBLEM));
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
			uninit(error(LIBC_FIND_PROBLEM));
		}
	}
	if (stat(libc_path, &buf) != 0) {
		uninit(error(LIBC_ACCESS_PROBLEM));
	}
	const char * libncurses_path;
	if (config_lookup_string(&config, "libncurses", &libncurses_path) == 0) {
		libncurses_path = getenv("LIBNCURSES");
		if (libncurses_path == NULL) {
			uninit(error(LIBNCURSES_FIND_PROBLEM));
		}
	}
	if (stat(libncurses_path, &buf) != 0) {
		uninit(error(LIBNCURSES_ACCESS_PROBLEM));
	}

	/*
	Loads functions from dynamically linked libraries.
	*/
	{
		void * handle = dlopen(libc_path, RTLD_LAZY);//requires either RTLD_LAZY or RTLD_NOW
		if (handle == NULL) {
			uninit(error(LIBC_PROBLEM));
		}
		um_unlink = (UNLINK )dlsym(handle, "unlink");
		um_time = (TIME )dlsym(handle, "time");
		um_localtime = (LOCALTIME )dlsym(handle, "localtime");
		um_srandom = (SRANDOM )dlsym(handle, "srandom");
		um_random = (RANDOM )dlsym(handle, "random");
		um_ioctl = (IOCTL )dlsym(handle, "ioctl");
		dlclose(handle);
	}
	{
		void * handle = dlopen(libncurses_path, RTLD_LAZY);
		if (handle == NULL) {
			uninit(error(LIBNCURSES_PROBLEM));
		}
		um_init_pair = (INIT_PAIR )dlsym(handle, "init_pair");
		um_wrefresh = (WREFRESH )dlsym(handle, "wrefresh");
		um_wgetch = (WGETCH )dlsym(handle, "wgetch");
		dlclose(handle);
	}

	/*
	Prevents reloading libraries for child processes.
	*/
	if (unsetenv("LD_PRELOAD") != 0) {
		warning(LD_PRELOAD_UNSET_PROBLEM);
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
	rows = MIN(MAX(ROWS_MIN, rows), ROWS_MAX);
	cols = MIN(MAX(COLS_MIN, cols), COLS_MAX);

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
	states++;//reserves space for the zero state

	/*
	Finds the replacement string.

	The configuration file is first parsed and
	the default string is then guessed.
	*/
	char replacement;
	const char * replacement_string;
	if (config_lookup_string(&config, "iterator", &replacement_string) == 0) {
		warning(CONFIG_ITERATOR_PROBLEM);
		replacement = default_replacement;
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
		input_path = default_input_name;
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
		output_path = default_output_name;
	}
	{
		char ** output_files = malloc(states*sizeof (*output_files));
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
	}
	output_streams = malloc(states*sizeof (*output_streams));
	for (unsigned int index = 0; index < states; index++) {
		if (output_files[index] != NULL) {
			if (stat(output_files[index], &buf) == 0) {
				warning(OUTPUT_OVERWRITE_PROBLEM);
			}
			else {
				output_streams[index] = fopen(output_files[index], "wb");
				if (output_stream == NULL) {
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
		STDSTR(new_error_stream, default_error_name);
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
				STDSTR(new_error_stream, default_error_name);
			}
		}
	}
	const char * warning_path;
	if (config_lookup_string(&config, "warnings", &warning_path) == 0) {
		warning(CONFIG_WARNING_LOG_PROBLEM);
		STDSTR(new_warning_stream, default_warning_name);
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
				STDSTR(new_warning_stream, default_warning_name);
			}
		}
	}
	const char * note_path;
	if (config_lookup_string(&config, "notes", &note_path) == 0) {
		warning(CONFIG_NOTE_LOG_PROBLEM);
		STDSTR(new_note_stream, default_note_name);
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
				STDSTR(new_note_stream, default_note_name);
			}
		}
	}
	const char * call_path;
	if (config_lookup_string(&config, "calls", &call_path) == 0) {
		warning(CONFIG_CALL_LOG_PROBLEM);
		STDSTR(new_call_stream, default_call_name);
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
				STDSTR(new_call_stream, default_call_name);
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
	const char * shm_path;
	if (config_lookup_string(&config, "shm", &shm_path) == 0) {
		warning(CONFIG_SHM_PROBLEM);
		shm_file = malloc(strlen(default_shm_name)+1);
		strcpy(shm_file, default_shm_name);
	}
	else {
		shm_file = malloc(strlen(shm_path)+1);
		strcpy(shm_file, shm_path);
	}

	/*
	Unloads the configuration file.

	The memory allocated by the <code>config_lookup_</code> calls is automatically deallocated.
	*/
	config_destroy(&config);
}

/**
Very important temporary variables.
**/
int globstate = 1;
time_t current_time = 0;//0x7fe81780
record_t record;

/**
Redirects calls from injected instructions.
**/
void injector() {
	seed(current_time);
	add_seed_frame(&record, current_time);
	wrefresh(win);
}

/**
Seeds the ARC4 of the executable.

The seeding is simulated and the simulated state is then copied.
**/
void seed(const int seed) {
	arc4_i = 0;
	arc4_j = 0;
	um_srandom(seed);
	sarc4(um_random());
	for (unsigned int i = 0; i < ARC4_CALLS_AUTO_LOAD; i++) arc4();
	memcpy(ARC4_S, arc4_s, sizeof (arc4_s));
	//increment count here
}

/**
Catches signals or something.
**/
void handle_parent(const int sig) {
	if (sig == SIGUSR1) {
		fprintfl(warning_stream, "#%u caught USR1.", getpid());
	}
	else if (sig == SIGUSR2) {
		fprintfl(warning_stream, "#%u caught USR2.", getpid());
	}
	else if (sig == SIGTERM) {
		fprintfl(warning_stream, "#%u caught TERM.", getpid());
		uninit_parent(NO_PROBLEM);
	}
}

/**
Catches signals or something.
**/
void handle_child(const int sig) {
	if (sig == SIGUSR1) {
		fprintfl(warning_stream, "#%u caught USR1.", getpid());
	}
	else if (sig == SIGUSR2) {
		fprintfl(warning_stream, "#%u caught USR2.", getpid());
	}
	else if (sig == SIGTERM) {
		fprintfl(warning_stream, "#%u caught TERM.", getpid());
		uninit_child(NO_PROBLEM);
	}
}

/**
Initializes this process.
**/
bool initialized = FALSE, actually_initialized = FALSE;
void init_parent() {
	initialized = TRUE;

	init_config();

	init_record(&record);

	/*
	Injects Assembly instructions to disable the save function of the executable.
	*/
	//inject_save(&injector);

	actually_initialized = TRUE;

	/*if (signal(SIGWINCH, dreamcatcher) == SIG_ERR) fprintfl(note_stream, "No no resizing!");
	if (signal(SIGCONT, terminator) == SIG_ERR) fprintfl(note_stream, "Can't catch CONT.");
	if (signal(SIGINT, terminator) == SIG_ERR) fprintfl(note_stream, "Can't stop!");
	if (signal(SIGTERM, terminator) == SIG_ERR) fprintfl(note_stream, "Can't catch anything.");*/

	shm_init(shm_file);

	struct sigaction act;
	act.sa_handler = terminator;
	act.sa_flags = 0;
	if (sigaction(SIGTERM, &act, NULL) != 0) {
		fprintfl(error_stream, "Can't catch TERM.");
	}

	pid_t pid = fork();
	if (pid == -1) {
		error(FORK_PROBLEM);
	}
	else if (pid == 0) {//child
		shm_init();
		shm->pids[0] = getpid();
	}
	else {//parent
		shm->ppid = getpid();
		struct sigaction act;
		act.sa_handler = handle_signals;
		act.sa_flags = SA_NOCLDSTOP | SA_NOCLDWAIT;
		if (sigaction(SIGUSR1, &act, NULL) != 0) {
			fprintfl(note_stream, "Can't catch CONT.");
		}
		if (sigaction(SIGUSR2, &act, NULL) != 0) {
			fprintfl(note_stream, "Can't catch CONT.");
		}
		fprintf(stderr, "The parent process seems to have fallen asleep. Use Ctrl C to wake it up.\n");
		sigset_t mask;
		sigfillset(&mask);
		sigdelset(&mask, SIGUSR1);
		sigwait(&mask);
		fprintf(stderr, "Quitting...\n");
		uninit(error(NO_PROBLEM/*_MATE*/));
	}
}

#endif
