/**
Serves as a loader for the executable.
**/
#ifndef LOADER_C
#define LOADER_C

#include <stdlib.h>//TODO get rid of the unnecessary
#include <stdio.h>
#include <curses.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <libconfig.h>

#include "util.h"
#include "problem.h"
#include "log.h"
#include "put.h"
#include "config.h"
#include "adom.h"
#include "loader.h"

int rows;
int cols;
char * input_file;
char * output_file;
FILE * error_stream;
FILE * warning_stream;
FILE * note_stream;
FILE * call_stream;

/**
Declares the unmodified versions of the functions that are overloaded.
**/
INIT_PAIR real_init_pair;//TODO get rid of the unnecessary
WCLEAR real_wclear;
WREFRESH real_wrefresh; 
WMOVE real_wmove;
WADDCH real_waddch;
WADDNSTR real_waddnstr;
WINCH real_winch;
WGETCH real_wgetch;
WGETNSTR real_wgetnstr;
UNLINK real_unlink;
TIME real_time;
LOCALTIME real_localtime;
RANDOM real_random;
SRANDOM real_srandom;
IOCTL real_ioctl;

/**
Very important temporary variables.
**/
int globstate = 1;
time_t current_time = 0;//0x7fe81780

/**
Represents a recorded frame.

Three kinds of inputs exist:
<pre>
enum input_e {
	NO_INPUT,
	KEY_INPUT,
	TIME_INPUT,
	SEED_INPUT
};
typedef enum input_e input_t;
</pre>
<code>KEY_INPUT</code> represents pressing a key,
<code>TIME_INPUT</code> represents changing the system time and
<code>SEED_INPUT</code> represents reseeding the random number generator.
The variables required to represent a frame depend on the inputs:
<pre>
struct frame_s {
	input_t input;
	int key;
	time_t time;
	unsigned char duration;
	struct frame_s * next;
};
typedef struct frame_s frame_t;
</pre>
Since <code>time_t</code> can be treated as an <code>int</code> or a <code>long</code>,
only <code>KEY_INPUT</code> inputs are visible and
<code>TIME_INPUT</code> is only used with the next <code>SEED_INPUT</code>
the struct can be condensed:
<pre>
struct frame_s {
	int duration;//duration == 0 ? input = KEY_INPUT : input = SEED_INPUT
	int value;//duration != 0 ? key = value : time += value
	struct frame_s * next;
};
typedef struct frame_s frame_t;
</pre>
Thus only two <code>int</code>s are needed.
**/
struct frame_s {
	int duration;
	int value;
	struct frame_s * next;
};
typedef struct frame_s frame_t;

frame_t * first_frame = NULL;
frame_t * last_frame = NULL;
unsigned int frame_count = 0;

/**
Adds a frame.
**/
frame_t * add_frame(int duration, int value) {
	frame_t * new_frame = malloc(sizeof (frame_t));
	new_frame->duration = duration;
	new_frame->value = value;
	new_frame->next = NULL;
	if (first_frame == NULL) {//the first
		first_frame = new_frame;
	}
	else {//the rest
		last_frame->next = new_frame;
	}
	last_frame = new_frame;
	frame_count++;
	return new_frame;
}

/**
Removes all frames.
**/
void remove_frames() {
	frame_t * current_frame = first_frame;
	first_frame = NULL;
	last_frame = NULL;
	while (current_frame != NULL) {
		frame_t * old_frame = current_frame;
		current_frame = current_frame->next;
		free(old_frame);
	}
}

time_t last_time;

/**
Adds a <code>KEY_INPUT</code> frame.
**/
frame_t * add_key_frame(unsigned int duration, int input) {
	return add_frame((int )duration, input);
}

/**
Adds a <code>TIME_INPUT</code> and <code>SEED_INPUT</code> frame.
**/
frame_t * add_seed_frame(time_t time) {
	if (first_frame == NULL) {
		last_time = current_time;
	}
	const time_t step_time = current_time-last_time;
	last_time = current_time;
	return add_frame(0, (int )step_time);
}

/**
Called from injected Assembly (details arrive later).
**/
void internal() {
	fprintfl(note_stream, ":)");
}

/**
Outputs the frames (for recording).
**/
int barf() {
	FILE * output_stream = fopen(output_file, "wb");
	if (output_stream == NULL) {
		return error(OUTPUT_ACCESS_PROBLEM);
	}
	int result = 0;
	frame_t * current_frame = first_frame;
	while (current_frame != NULL) {
		result += fwrite(&current_frame->duration, sizeof (current_frame->duration), 1, output_stream);
		result += fwrite(&current_frame->value, sizeof (current_frame->value), 1, output_stream);
		current_frame = current_frame->next;
	}
	fclose(output_stream);
	return result;
}

/**
Inputs the frames (for playback).
**/
int slurp() {
	FILE * input_stream = fopen(input_file, "rb");
	if (input_stream == NULL) {
		return error(INPUT_ACCESS_PROBLEM);
	}
	int result = 0;
	while (TRUE) {
		int subresult = 0;
		int duration;
		int value;
		subresult += fread(&duration, sizeof (duration), 1, input_stream);
		subresult += fread(&value, sizeof (value), 1, input_stream);
		if (subresult == 0) break;
		add_frame(duration, value);
	}
	if (feof(input_stream)) /*error*/;
	clearerr(input_stream);
	fclose(input_stream);
	return result;
}

/**
Logs the ARC4 status.
**/
int printrl() {
	FILE * random_stream = fopen(TEMPORARY_ACTUAL_PATH, "wb");
	int result = 0;
	if (random_stream != NULL) {
		result += fwrite(ARC4_S, sizeof (unsigned char), 0x100, random_stream);
		result += fwrite(ARC4_I, sizeof (unsigned char), 1, random_stream);
		result += fwrite(ARC4_J, sizeof (unsigned char), 1, random_stream);
	}
	fclose(random_stream);
	return result;
}
int printsrl() {
	FILE * random_stream = fopen(TEMPORARY_SIM_PATH, "wb");
	int result = 0;
	if (random_stream != NULL) {
		result += fwrite(arc4_s, sizeof (unsigned char), 0x100, random_stream);
		result += fwrite(&arc4_i, sizeof (unsigned char), 1, random_stream);
		result += fwrite(&arc4_j, sizeof (unsigned char), 1, random_stream);
	}
	fclose(random_stream);
	return result;
}

/**
Seeds the ARC4 of the executable.

Seeding can be simulated:
<pre>
real_srandom(time(NULL));
sarc4(real_random());
</pre>
**/
void seed(const int seed) {
	//SARC4_TIME();
	arc4_i = 0;
	arc4_j = 0;
	real_srandom(seed);
	sarc4(real_random());
	//memcpy(ARC4_S, arc4_s, sizeof (arc4_s));
}

/**
Shares memory or something.
**/
struct shm_s {
	int term;//if needed to fix detaching
	int pids[SAVE_STATES];
};
typedef struct shm_s shm_t;
int shm_fd;
shm_t * conf;
void shmup() {
	bool first = TRUE;

	fprintfl(note_stream, "Started to map shared memory.");
	shm_fd = shm_open(SHM_PATH, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
	if (shm_fd < 0) {
		first = FALSE;
	}
	shm_fd = shm_open(SHM_PATH, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if (shm_fd < 0) {
		fprintfl(note_stream, "Failed to map shared memory.");
		return;//error here
	}

	ftruncate(shm_fd, (off_t )sizeof (shm_t));

	conf = (shm_t * )mmap(NULL, sizeof (shm_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (conf == MAP_FAILED) {
		fprintfl(note_stream, "Failed to map shared memory.");
		return;//error here
	}

	if (first) {
		for (int index = 0; index < SAVE_STATES; index++) conf->pids[index] = 0;
		fprintfl(note_stream, "Mapped shared memory.");
	}
	else fprintfl(note_stream, "Remapped shared memory.");
}

void dreamcatcher(const int signo) {
	fprintfl(note_stream, "Somehow caught \"%s\".", strsignal(signo));
}

/**
Initializes this process.
**/
bool initialized = FALSE;
int initialize() {
	struct stat buf;

	error_stream = stderr;
	warning_stream = stderr;
	note_stream = stderr;
	call_stream = NULL;

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
	Loads functions from dynamically linked libraries.
	*/
	{
		void * handle = dlopen(libc_path, RTLD_LAZY);//requires either RTLD_LAZY or RTLD_NOW
		if (handle == NULL) {
			exit(error(LIBC_PROBLEM));
		}
		real_unlink = (UNLINK )dlsym(handle, "unlink");
		real_time = (TIME )dlsym(handle, "time");
		real_localtime = (LOCALTIME )dlsym(handle, "localtime");
		real_random = (RANDOM )dlsym(handle, "random");
		real_srandom = (SRANDOM )dlsym(handle, "srandom");
		real_ioctl = (IOCTL )dlsym(handle, "ioctl");
		//dlclose(handle);
	}
	{
		void * handle = dlopen(libncurses_path, RTLD_LAZY);
		if (handle == NULL) {
			exit(error(LIBNCURSES_PROBLEM));
		}
		real_init_pair = (INIT_PAIR )dlsym(handle, "init_pair");
		real_wclear = (WCLEAR )dlsym(handle, "wclear");
		real_wrefresh = (WREFRESH )dlsym(handle, "wrefresh");
		real_wmove = (WMOVE )dlsym(handle, "wmove");
		real_waddch = (WADDCH )dlsym(handle, "waddch");
		real_waddnstr = (WADDNSTR )dlsym(handle, "waddnstr");
		real_winch = (WINCH )dlsym(handle, "winch");
		real_wgetch = (WGETCH )dlsym(handle, "wgetch");
		real_wgetnstr = (WGETNSTR )dlsym(handle, "wgetnstr");
		//dlclose(handle);
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
		rows = 25;
	}
	if (config_lookup_int(&config, "cols", &cols) == 0) {
		warning(CONFIG_COL_PROBLEM);
		cols = 80;
	}

	/*
	Opens the put streams.

	The configuration file is first parsed and
	the existence of the put file is then checked.
	*/
	const char * input_path;
	if (config_lookup_string(&config, "input", &input_path) == 0) {
		warning(CONFIG_INPUT_PROBLEM);
	}
	else {
		if (stat(input_path, &buf) == 0) {
			warning(INPUT_FIND_PROBLEM);
		}
	}
	const char * output_path;
	if (config_lookup_string(&config, "output", &output_path) == 0) {
		warning(CONFIG_OUTPUT_PROBLEM);
	}
	else {
		if (stat(output_path, &buf) == 0) {
			warning(OUTPUT_OVERWRITE_PROBLEM);
		}
	}
	note(LOG_CHANGE_PROBLEM);
	input_file = malloc(strlen(input_path)+1);
	strcpy(input_file, input_path);
	output_file = malloc(strlen(output_path)+1);
	strcpy(output_file, output_path);

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
		new_error_stream = stderr;
	}
	else {
		if (strcmp(error_path, "stdout") == 0) {
			new_error_stream = stdout;
		}
		else if (strcmp(error_path, "stderr") == 0) {
			new_error_stream = stderr;
		}
		else {
			if (stat(error_path, &buf) == 0) {
				note(ERROR_LOG_OVERWRITE_PROBLEM);
			}
			new_error_stream = fopen(error_path, "w");
			if (new_error_stream == NULL) {
				warning(ERROR_LOG_ACCESS_PROBLEM);
				new_error_stream = stderr;
			}
		}
	}
	const char * warning_path;
	if (config_lookup_string(&config, "warnings", &warning_path) == 0) {
		warning(CONFIG_WARNING_LOG_PROBLEM);
		new_warning_stream = stderr;
	}
	else {
		if (strcmp(warning_path, "stdout") == 0) {
			new_warning_stream = stdout;
		}
		else if (strcmp(warning_path, "stderr") == 0) {
			new_warning_stream = stderr;
		}
		else {
			if (stat(warning_path, &buf) == 0) {
				note(WARNING_LOG_OVERWRITE_PROBLEM);
			}
			new_warning_stream = fopen(warning_path, "w");
			if (new_warning_stream == NULL) {
				warning(WARNING_LOG_ACCESS_PROBLEM);
				new_warning_stream = stderr;
			}
		}
	}
	const char * note_path;
	if (config_lookup_string(&config, "notes", &note_path) == 0) {
		new_note_stream = stderr;
		warning(CONFIG_NOTE_LOG_PROBLEM);
	}
	else {
		if (strcmp(note_path, "stdout") == 0) {
			new_note_stream = stdout;
		}
		else if (strcmp(note_path, "stderr") == 0) {
			new_note_stream = stderr;
		}
		else {
			if (stat(note_path, &buf) == 0) {
				note(NOTE_LOG_OVERWRITE_PROBLEM);
			}
			new_note_stream = fopen(note_path, "w");
			if (new_note_stream == NULL) {
				warning(NOTE_LOG_ACCESS_PROBLEM);
				new_note_stream = stderr;
			}
		}
	}
	const char * call_path;
	if (config_lookup_string(&config, "calls", &call_path) == 0) {
		warning(CONFIG_CALL_LOG_PROBLEM);
		new_call_stream = NULL;
	}
	else {
		if (strcmp(call_path, "stdout") == 0) {
			new_call_stream = stdout;
		}
		else if (strcmp(call_path, "stderr") == 0) {
			new_call_stream = stderr;
		}
		else {
			if (stat(call_path, &buf) == 0) {
				note(CALL_LOG_OVERWRITE_PROBLEM);
			}
			new_call_stream = fopen(call_path, "w");
			if (new_call_stream == NULL) {
				warning(CALL_LOG_ACCESS_PROBLEM);
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
	Unloads the configuration file.

	The memory allocated by <code>config_lookup_string</code> calls is automatically deallocated.
	*/
	config_destroy(&config);

	//crap follows
	real_unlink("/dev/shm/adom-tas");//What is portable code?
	shmup();
	conf->pids[0] = getpid();

	void * CHEESE_MAGIC = (void * )0x08090733;
	/*
	This was an attempt to test the system on an x64.
	*/
	/*void * handle = dlopen(LIBRARY_PATH, RTLD_LAZY);//hopefully unnecessary
	if (handle == NULL) exit(error(DLOPEN_LIBC_ERROR));
	void * internal_address = dlsym(handle, "internal");*/
	unsigned int f = (unsigned int )&internal-(unsigned int )CHEESE_MAGIC;
	fprintfl(note_stream, "Somehow found 0x%08x-0x%08x = 0x%08x.", (unsigned int )&internal, 0x08090733, f);
	internal();
	unsigned char instructions[10];//TODO document
	instructions[0] = 0xe8;//CALL internal() 0x08090733->&internal
	instructions[1] = (unsigned char )((f>>0x00)&0xff);
	instructions[2] = (unsigned char )((f>>0x08)&0xff);
	instructions[3] = (unsigned char )((f>>0x10)&0xff);
	instructions[4] = (unsigned char )((f>>0x18)&0xff);
	instructions[5] = 0xe9;//JMP out of here
	instructions[6] = 0xf3;
	instructions[7] = 0x00;
	instructions[8] = 0x00;
	instructions[9] = 0x00;
	//redirects S somewhere
	void * location = (void * )0x0809072a;//conjurations and wizardry
	if (mprotect(PAGE(location), PAGE_SIZE(sizeof (instructions)), PROT_READ|PROT_WRITE|PROT_EXEC) == 0)
		memcpy(location, instructions, sizeof (instructions));//TODO make sure it's patching the right instructions
	else fprintfl(note_stream, ":(");

	if (signal(SIGWINCH, dreamcatcher) == SIG_ERR) fprintfl(note_stream, "No no resizing!");

	initialized = TRUE;
	return 0;//Why?
}

void uninitialize() {
	/*
	Deallocates things.
	*/
	free(input_file);
	free(output_file);

	/*
	Closes the log streams.
	*/
	fclose(error_stream);
	fclose(warning_stream);
	fclose(note_stream);
	fclose(call_stream);

	/*
	Closes the shm.
	*/
	close(shm_fd);

	exit(0);
}

bool tired = TRUE;
void continuator(const int signo) {
	if (signo == SIGCONT) {
		fprintfl(note_stream, "Caught CONT.");
		tired = FALSE;
	}
}

/**
Saves the game to memory.
**/
void save(const int state) {
	pid_t pid = fork();//returns 0 in child, process id of child in parent, -1 on error
	shmup();
	if (signal(SIGTERM, dreamcatcher) == SIG_ERR) fprintfl(note_stream, "Can't catch anything.");
	if (signal(SIGINT, dreamcatcher) == SIG_ERR) fprintfl(note_stream, "Can't catch anything.");
	if (signal(SIGHUP, dreamcatcher) == SIG_ERR) fprintfl(note_stream, "Can't catch anything.");
	if (signal(SIGQUIT, dreamcatcher) == SIG_ERR) fprintfl(note_stream, "Can't catch anything.");
	if (signal(SIGTRAP, dreamcatcher) == SIG_ERR) fprintfl(note_stream, "Can't catch anything.");
	if (signal(SIGABRT, dreamcatcher) == SIG_ERR) fprintfl(note_stream, "Can't catch anything.");
	if (signal(SIGSTOP, dreamcatcher) == SIG_ERR) fprintfl(note_stream, "Can't catch anything.");
	if (signal(SIGTTOU, dreamcatcher) == SIG_ERR) fprintfl(note_stream, "Can't catch anything.");
	if (pid != (pid_t )NULL) {//parent
		if (conf->pids[state] != 0) {//removes an old save
			kill(conf->pids[state], SIGKILL);//kills the parent of another process and causes weird problems
		}
		conf->pids[state] = getpid();
		if (signal(SIGCONT, continuator) == SIG_ERR) fprintfl(note_stream, "Can't catch CONT.");
		//printf("<%d fell asleep>", (int )getpid()); fflush(stdout);
		struct timespec req;
		req.tv_sec = (time_t )0;
		req.tv_nsec = 1000000000l/16l;//extern this
		while (tired) nanosleep(&req, NULL);
		//printf("<%d woke up>", (int )getpid()); fflush(stdout);
	}
	else {//child
		//printf("<%d is ready>", (int )getpid()); fflush(stdout);
		conf->pids[0] = getpid();
	}
}

/**
Loads the game from memory.
**/
void load(const int state) {
	//printf("<%d poked %d>", (int )getpid(), (int )conf->pids[state]); fflush(stdout);
	if (conf->pids[state] != 0) {
		kill(conf->pids[state], SIGCONT);
		//printf("<%d killed %d>", (int )getpid(), (int )conf->pids[0]); fflush(stdout);
		const int zorg = conf->pids[0];
		conf->pids[0] = conf->pids[state];
		conf->pids[state] = 0;
		kill(zorg, SIGKILL);
	}
}

/**
Annotates (and more) overloaded functions.

The first call loads the libraries.
**/
//#define OVERLOAD if (!initialized) initialized = initialize();
#define OVERLOAD if (!initialized) initialize();

short pairs = 0;

/**
Overloads wgetch with a simple log wrapper.
**/
bool was_meta = FALSE;//not good
int was_colon = FALSE;//worse
bool condensed = FALSE;
bool playbacking = FALSE;
frame_t * playback_frame;
char codeins[7];
int wgetch(WINDOW * win) { OVERLOAD//bloat
	call("wgetch(0x%08x).", (unsigned int )win);
	if (playbacking) {
		if (playback_frame != NULL) {//TODO move this
			if (playback_frame->duration == 0) {
				current_time += playback_frame->value;
				playback_frame = playback_frame->next;
				return 0;
			}
			else {
				struct timespec req;
				bool out_of_variable_names = FALSE;
				if (playback_frame->duration >= 16) out_of_variable_names = TRUE;
				req.tv_sec = (time_t )(out_of_variable_names ? playback_frame->duration : 0);
				req.tv_nsec = out_of_variable_names ? 0l : 1000000000l/16*playback_frame->duration;
				nanosleep(&req, NULL);
				const int yield = playback_frame->value;
				playback_frame = playback_frame->next;
				return yield;
			}
		}
	}
	int key = real_wgetch(win);
	if (key == 0x110) {
		if (frame_count == 0) {//move to playback
			slurp();
			playbacking = TRUE;
			playback_frame = first_frame;
		}
		else condensed = !condensed;
		wrefresh(win);
		return 0;
	}
	else if (key == KEY_F(9)) {//saves
		save(globstate);
		wrefresh(win);
		return 0;
	}
	else if (key == KEY_F(10)) {//loads
		load(globstate);
		wrefresh(win);
		return 0;//redundant
	}
	else if (key == KEY_F(36)) {//changes the state (Ctrl F12 now)
		globstate = globstate%(10-1)+1;//++
		//globstate = (globstate-2)%(10-1)+1;//--
		wrefresh(win);
		return 0;
	}
	else if (key == KEY_F(11)) {//changes the time
		current_time++;
		wrefresh(win);
		return 0;
	}
	else if (key == KEY_F(12)) {//saves the seed frame (later S)
		seed(current_time);//TODO fix
		add_seed_frame(current_time);
		wrefresh(win);
		return 0;
	}
	else if (key == '_') {//dumps everything
		barf();//TODO move
		printrl();
		printsrl();
		return 0;
	}
	else if (key == -12) {//a failed attempt to simulate the executable initializing the random number genrator
		seed(current_time);
		for (int i = 0; i < 0xffff; i++) {
			for (int j = 0; j < 4; j++) arc4();
			if (harc4(arc4_s) == harc4(ARC4_S)) {
				fprintfl(note_stream, "Built the hash %d:0x%08x.", i, harc4());
				break;
			}
		}
		wrefresh(win);
		return 0;
	}
	if (!was_meta && !was_colon && (key == 0x3a || key == 'w')) was_colon = key == 0x3a ? 1 : 2;//booleans are fun like that
	else if (!was_meta && key == 0x1b) was_meta = TRUE;
	else {
		char code[4];
		strcpy(codeins, "");
		if (was_colon) {
			key_code(code, was_colon == 1 ? 0x3a : 'w');
			strcat(codeins, code);
		}
		if (was_meta) {
			key_code(code, 0x1b);
			strcat(codeins, code);
		}
		was_colon = FALSE;
		was_meta = FALSE;
		key_code(code, key);
		strcat(codeins, code);//TODO turn this into a macro
	}
	unsigned int duration = 8;
	add_key_frame(duration, key);//meta, colon and w are undisplayed but still recorded (for now)
	//wrefresh(win);
	return key;
}

/**
Overloads random with a simple log wrapper.
**/
long random() { OVERLOAD
	call("random().");
	return real_random();
}

/**
Overloads srandom with a simple log wrapper.
**/
void srandom(unsigned int seed) { OVERLOAD
	call("srandom(%u).", seed);
	real_srandom(seed);
}

/**
Overloads init_pair with a simple log wrapper.
**/
int init_pair(short pair, short f, short b) { OVERLOAD
	call("init_pair(%d, %d, %d).", pair, f, b);
	pairs++;
	return real_init_pair(pair, f, b);
}

/**
Overloads time with a simple log wrapper.
**/
time_t time(time_t * t) { OVERLOAD
	call("time(0x%08x).", (unsigned int )t);
	const time_t n = (time_t )current_time;
	if (t != NULL) *t = n;
	return n;
}

/**
Overloads localtime with a simple log wrapper.
**/
struct tm * localtime(const time_t * timep) { OVERLOAD
	call("localtime(0x%08x).", (unsigned int )timep);
	return gmtime(timep);//timezones are useless anyway
}

/**
Overloads wrefresh with a "simple log wrapper".
**/
int wrefresh(WINDOW * win) { OVERLOAD
	call("wrefresh(0x%08x).", (unsigned int )win);

	int x, y;
	attr_t attrs; attr_t * _attrs = &attrs;//suppresses a warning caused by a curses bug
	short pair; short * _pair = &pair;
	getyx(win, y, x);
	wattr_get(win, _attrs, _pair, NULL);

	short ws_pair = pairs;
	#define ws_INIT_PAIR(b) \
		real_init_pair(ws_pair, COLOR_BLACK, b);\
		ws_pair++;
	ws_INIT_PAIR(COLOR_RED);
	ws_INIT_PAIR(COLOR_YELLOW);
	ws_INIT_PAIR(COLOR_GREEN);
	ws_INIT_PAIR(COLOR_CYAN);
	ws_INIT_PAIR(COLOR_BLUE);
	ws_INIT_PAIR(COLOR_MAGENTA);

	int ws_x = TERM_COL, ws_y = TERM_ROW-1;
	char ws_str[TERM_COL], ws_buf[TERM_COL];
	#define ws_ADDSTR(format, length, arg) \
		ws_pair--;\
		wattrset(win, COLOR_PAIR(ws_pair));\
		snprintf(ws_str, (size_t )TERM_COL, format, arg);\
		snprintf(ws_buf, (size_t )((condensed ? 1 : length)+1), "%-*s", length, ws_str);\
		ws_x -= length;\
		mvaddnstr(ws_y, ws_x, ws_buf, TERM_COL-ws_x);\
		ws_x--;
	ws_ADDSTR("S: %d", 4, globstate);
	ws_ADDSTR("T: 0x%08x", 13, (unsigned int )current_time);
	ws_ADDSTR("R: 0x%08x", 13, harc4(ARC4_S));
	ws_ADDSTR("G: %d", 13, TURNS);
	ws_ADDSTR("F: %d", 13, frame_count);
	ws_ADDSTR("I: %s", 9, codeins);

	char some[TERM_COL];//a hack
	strcpy(some, "P:");
	for (int index = 0; index < SAVE_STATES; index++) {
		if (conf->pids != NULL) {
			char somer[TERM_COL];
			bool somery = conf->pids[index] != 0;
			sprintf(somer, "%s %c%d%c", some, somery ? '[' : '<', index, /*conf->pids[index],*/ somery ? ']' : '>');
			strcpy(some, somer);
		}
	}
	mvaddnstr(21, 10, some, TERM_COL-20);

	wattr_set(win, attrs, pair, NULL);
	wmove(win, y, x);
	const int result = real_wrefresh(win);

	return result;
}

/**
Tells sweet lies about the terminal size.
**/
int ioctl(int d, unsigned long request, ...) { OVERLOAD
	va_list	argp;
	va_start(argp, request);
	const void * arg = va_arg(argp, void * );
	call("ioctl(0x%08x, %lu, 0x%08x).", (unsigned int )d, request, (unsigned int )arg);
	const int result = real_ioctl(d, request, arg);
	if (request == TIOCGWINSZ) {
		struct winsize * size;
		size = (struct winsize * )arg;
		size->ws_row = rows;
		size->ws_col = cols;
	}
	va_end(argp);
	return result;
}

#endif
