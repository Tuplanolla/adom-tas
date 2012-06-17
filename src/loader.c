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
#include "adom.h"
#include "record.h"

int rows;
int cols;
int states;
char * shm_file;
char * input_file;
char ** output_files;
FILE * error_stream;
FILE * warning_stream;
FILE * note_stream;
FILE * call_stream;

typedef int (* UNLINK)(const char * path);
typedef int (* IOCTL)(int d, unsigned long request, ...);
typedef time_t (* TIME)(time_t * timer);
typedef struct tm * (* LOCALTIME)(const time_t * timep);
typedef void (* SRANDOM)(unsigned int seed);
typedef long (* RANDOM)();
typedef int (* INIT_PAIR)(short pair, short f, short b);
typedef int (* WREFRESH)(WINDOW * win);
typedef int (* WGETCH)(WINDOW * win);

UNLINK um_unlink = NULL;
IOCTL um_ioctl = NULL;
TIME um_time = NULL;
LOCALTIME um_localtime = NULL;
SRANDOM um_srandom = NULL;
RANDOM um_random = NULL;
INIT_PAIR um_init_pair = NULL;
WREFRESH um_wrefresh = NULL;
WGETCH um_wgetch = NULL;

/**
Very important temporary variables.
**/
int globstate = 1;
time_t current_time = 0;//0x7fe81780

/**
Redirects calls from injected instructions.
**/
void internal() {
	fprintfl(note_stream, ":)");
}

record_t record;

/**
Seeds the ARC4 of the executable.

Seeding can be simulated:
<pre>
um_srandom(time(NULL));
sarc4(um_random());
</pre>
**/
void seed(const int seed) {//simulated only (for now)
	arc4_i = 0;
	arc4_j = 0;
	um_srandom(seed);
	sarc4(um_random());
	for (unsigned int i = 0; i < ARC4_CALLS_AUTO_LOAD; i++) arc4();
	memcpy(ARC4_S, arc4_s, sizeof (arc4_s));
}

/**
Holds pointers to the shared memory.

The contents come after the pointers:
<pre>
           ,--------->---------.
[ ppid ][ * pids ][ ** scrs ][ pid[0] pid[1] ... pid[states] ][ * scr[0] * scr[1] ... * scr[states] ]
                      `------------------->--------------------´
</pre>

@var ppid The process identifier of the parent process.
@var pids A pointer to the process identifiers of the child processes.
@var screens A pointer to the screens of the child processes.
**/
struct shm_s {
	int ppid;
	int * pids;
	chtype ** scrs;
};
typedef struct shm_s shm_t;
int shmid;
key_t key;
shm_t * shm;

/**
Catches signals or something.
**/
void dreamcatcher(const int signo) {
	fprintfl(note_stream, "Somehow caught \"%s\".", strsignal(signo));
}

/**
Uninitializes this process.

Contains unnecessary checks.
**/
void uninit(problem_t code) {
	//shmdetach();

	/*
	Closes the log streams.
	*/
	if (error_stream != NULL) fclose(error_stream);
	if (warning_stream != NULL) fclose(warning_stream);
	if (note_stream != NULL) fclose(note_stream);
	if (call_stream != NULL) fclose(call_stream);

	/*
	Exits (not gracefully).
	*/
	exit(code);
}

void shmattach() {//these things make the best function names
	const size_t size = sizeof (*shm)+states*sizeof (*shm->pids)+states*rows*cols*sizeof (**shm->scrs);
	shmid = shmget(key, size, SHM_R | SHM_W);
	if (shmid == -1) {
		uninit(error(NO_PROBLEM));//SHM_GET_ERROR
	}
	shm = shmat(shmid, NULL, 0);
	if (shm == (void * )-1) {
		uninit(error(NO_PROBLEM));//SHM_AT_ERROR
	}
	shm->pids = (int * )((int )shm+sizeof (*shm));
	shm->scrs = (chtype ** )((int )shm+sizeof (*shm)+states*sizeof (*shm->pids));
}

/**
Catches signals or something.
**/
bool tired = TRUE;
void continuator(const int signo) {
	if (signo == SIGCONT) {
		fprintfl(warning_stream, "[%06d::catch(CONT)]", (unsigned short )getpid()); fflush(stdout);
		tired = FALSE;
	}
}
void terminator(const int signo) {
	if (signo == SIGTERM) {
		fprintfl(warning_stream, "[%06d::catch(TERM)]", (unsigned short )getpid()); fflush(stdout);
		uninit(NO_PROBLEM);
	}
}

/**
Initializes this process.
**/
bool initialized = FALSE, actually_initialized = FALSE;
void init() {
	initialized = TRUE;

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
	Finds the replacement character.

	The configuration file is first parsed and
	the default character is then guessed.
	*/
	char replacement;
	const char * replacement_string;
	if (config_lookup_string(&config, "index", &replacement_string) == 0) {
		warning(NO_PROBLEM);//SOME_PROBLEM
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
	else {
		if (stat(input_path, &buf) == 0) {
			warning(INPUT_FIND_PROBLEM);
		}
	}
	const char * output_path;
	if (config_lookup_string(&config, "output", &output_path) == 0) {
		warning(CONFIG_OUTPUT_PROBLEM);
		output_path = default_output_name;
	}
	else {
		if (stat(output_path, &buf) == 0) {
			warning(OUTPUT_OVERWRITE_PROBLEM);
		}
	}
	input_file = malloc(strlen(input_path)+1);
	strcpy(input_file, input_path);
	output_files = malloc(states*sizeof (*output_files));
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
	{
		const size_t size = sizeof (*shm)+states*sizeof (*shm->pids)+states*rows*cols*sizeof (**shm->scrs);
		key = ftok(shm_path, hash((unsigned char * )default_project_name, strlen(default_project_name)));
		shmid = shmget(key, size, IPC_CREAT | SHM_R | SHM_W);
		if (shmid == -1) {
			uninit(error(NO_PROBLEM));//SHM_GET_ERROR
		}
		/*
		if (shmdt(shm) == -1) {
			perror("shmdt");
			exit(1);
		}
		if (shmctl(shmid, IPC_RMID, 0) == -1) {
			perror("shmctl");
			exit(1);
		}
		*/
	}

	/*
	Unloads the configuration file.

	The memory allocated by the <code>config_lookup_</code> calls is automatically deallocated.
	*/
	config_destroy(&config);

	init_record(&record);

	/*
	Injects Assembly instructions to disable the save function of the executable.
	*/
	bool inject = FALSE;
	if (!inject) goto hell;
	void * CHEESE_MAGIC = (void * )0x08090733;
	unsigned int f = (unsigned int )&internal-(unsigned int )CHEESE_MAGIC;
	fprintfl(note_stream, "Somehow found 0x%08x-0x%08x = 0x%08x.", (unsigned int )&internal, 0x08090733, f);
	unsigned char instructions[10];//TODO document
	instructions[0] = 0xe8;//CALL internal() 0x08090733->&internal
	instructions[1] = (unsigned char )((f>>0x00)&0xff);
	instructions[2] = (unsigned char )((f>>0x08)&0xff);
	instructions[3] = (unsigned char )((f>>0x10)&0xff);
	instructions[4] = (unsigned char )((f>>0x18)&0xff);
	instructions[5] = 0xe9;//JMP out of here
	instructions[6] = 0xf3;//points to RET
	instructions[7] = 0x00;
	instructions[8] = 0x00;
	instructions[9] = 0x00;
	void * location = (void * )0x0809072a;//conjurations and wizardry
	if (mprotect(PAGE(location), PAGE_SIZE(instructions), PROT_READ | PROT_WRITE | PROT_EXEC) == 0)
		memcpy(location, instructions, sizeof (instructions));//TODO make sure it's patching the right instructions
	else fprintfl(note_stream, ":(");
	hell:

	actually_initialized = TRUE;

	if (signal(SIGWINCH, dreamcatcher) == SIG_ERR) fprintfl(note_stream, "No no resizing!");
	if (signal(SIGCONT, terminator) == SIG_ERR) fprintfl(note_stream, "Can't catch CONT.");
	if (signal(SIGINT, terminator) == SIG_ERR) fprintfl(note_stream, "Can't stop!");
	if (signal(SIGTERM, terminator) == SIG_ERR) fprintfl(note_stream, "Can't catch anything.");

	shmattach();
	shm->ppid = 0;
	for (int index = 0; index < states; index++) {
		shm->pids[index] = 0;
	}

	pid_t pid = fork();//returns 0 in child, process id of child in parent, -1 on error
	if (pid == -1) exit(-1);
	if (pid != 0) {//parent
		shm->ppid = getpid();
		struct sigaction act;
		act.sa_handler = continuator;
		act.sa_flags = 0;
		if (sigaction(SIGCONT, &act, NULL) != 0) fprintfl(note_stream, "Can't catch CONT.");
		fprintf(stderr, "The parent process seems to have fallen asleep. Use Ctrl C to wake it up.\n");
		pause();
		fprintf(stderr, "Quitting...\n");
		uninit(NO_PROBLEM/*_MATE*/);
	}
	else {//child
		//const problem_t p = shmattach();
		//if (p != NO_PROBLEM) uninit(p);
		shmattach();
		shm->pids[0] = getpid();
	}
}

/**
Saves the game to memory.
**/
chtype ** screen;
void save(const int state) {
	int y, x;
	attr_t attrs; attr_t * _attrs = &attrs;
	short pair; short * _pair = &pair;
	getyx(stdscr, y, x);
	wattr_get(stdscr, _attrs, _pair, NULL);
	screen = malloc(rows*sizeof (chtype *));
	for (int row = 0; row < rows; row++) {
		chtype * subscreen = malloc(cols*sizeof (chtype));
		for (int col = 0; col < cols; col++) {
			subscreen[col] = mvinch(row, col);
		}
		screen[row] = subscreen;
	}
	fprintfl(warning_stream, "[%06d::fork()]", (unsigned short )getpid()); fflush(stdout);
	pid_t pid = fork();//returns 0 in child, process id of child in parent, -1 on error
	shmattach();
	struct sigaction act;
	act.sa_handler = continuator;
	act.sa_flags = 0;
	if (sigaction(SIGCONT, &act, NULL) != 0) fprintfl(note_stream, "Can't catch CONT.");
	if (pid == -1) uninit(error(NO_PROBLEM));
	if (pid != 0) {//parent
		if (shm->pids[state] != 0) {
			fprintfl(warning_stream, "[%06d::kill(%06d)]", (unsigned short )getpid(), (unsigned short )shm->pids[state]); fflush(stdout);
			kill(shm->pids[state], SIGKILL);
		}
		shm->pids[state] = getpid();
		fprintfl(warning_stream, "[%06d::stop()]", (unsigned short )getpid()); fflush(stdout);
		pause();
		fprintfl(warning_stream, "[%06d::continue()]", (unsigned short )getpid()); fflush(stdout);

		for (int row = 0; row < rows; row++) {
			for (int col = 0; col < cols; col++) {
				mvaddch(row, col, screen[row][col]);
			}
			free(screen[row]);
		}
		free(screen);
		wattr_set(stdscr, attrs, pair, NULL);
		wmove(stdscr, y, x);
		um_wrefresh(stdscr);
	}
	else {//child
		fprintfl(warning_stream, "[%06d::born(%06d)]", (unsigned short )getpid(), (unsigned short )getppid()); fflush(stdout);
		shm->pids[0] = getpid();
	}
}

/**
Loads the game from memory.
**/
void load(const int state) {
	if (shm->pids[state] != 0) {
		fprintfl(warning_stream, "[%06d::signal(%06d)]", (unsigned short )getpid(), (unsigned short )shm->pids[state]); fflush(stdout);
		kill(shm->pids[state], SIGCONT);
		const int zorg = shm->pids[0];
		fprintfl(warning_stream, "spid=%d == gpid=%d?", (unsigned short )getpid(), (unsigned short )shm->pids[0]); fflush(stdout);
		shm->pids[0] = shm->pids[state];
		shm->pids[state] = 0;
		fprintfl(warning_stream, "[%06d::kill(%06d)]", (unsigned short )getpid(), (unsigned short )zorg); fflush(stdout);
		//kill(zorg, SIGKILL);//kills two processes for an unknown reason
	}
}

/**
Annotates and initializes overloaded functions.
**/
#define OVERLOAD if (!initialized) init();

bool was_meta = FALSE;//not good
int was_colon = FALSE;//worse
bool condensed = FALSE;
bool playbacking = FALSE;
frame_t * playback_frame;
char codeins[7];

/**
Removes a file.

Intercepts removing the debug file if it exists.

@param path The path of the file to remove.
@return Zero if no errors occurred and something else otherwise.
**/
int unlink(const char * path) { OVERLOAD
	call("unlink(\"%s\").", path);
	if (strcmp(path, "ADOM.DBG") == 0) {
		//sleep(1);
		return 0;
	}
	return um_unlink(path);
}

/**
Controls the terminal.

Intercepts <code>TIOCGWINSZ</code> to always report a fixed size.
Intercepting <code>SIGWINCH</code> elsewhere is also required.

@param d An open file descriptor.
@param request A request conforming to <code>ioctl_list</code>.
@param ... A single pointer.
@return Zero if no errors occurred and something else otherwise.
**/
int ioctl(int d, unsigned long request, ...) { OVERLOAD
	va_list	argp;
	va_start(argp, request);
	const void * arg = va_arg(argp, void * );
	call("ioctl(0x%08x, 0x%08x, 0x%08x).", (unsigned int )d, (unsigned int )request, (unsigned int )arg);
	const int result = um_ioctl(d, request, arg);
	if (request == TIOCGWINSZ) {
		struct winsize * size;
		size = (struct winsize * )arg;
		size->ws_row = rows;
		size->ws_col = cols;
	}
	va_end(argp);
	return result;
}

/**
Returns the current system time.

Replaces the system time with a fixed time.

@param t The fixed time to return.
@return The fixed time.
**/
time_t time(time_t * t) { OVERLOAD
	call("time(0x%08x).", (unsigned int )t);
	if (t != NULL) *t = current_time;
	return current_time;//reduces entropy
}

/**
Converts a <code>time_t</code> to a broken-down <code>struct tm</code>.

Replaces <code>localtime</code> with <code>gmtime</code> and disregards timezones.

@param timep The <code>time_t</code> to convert.
@return The <code>struct tm</code>.
**/
struct tm * localtime(const time_t * timep) { OVERLOAD
	call("localtime(0x%08x).", (unsigned int )timep);
	return gmtime(timep);//reduces entropy
}

/**
Seeds the pseudorandom number generator.

@param seed The seed.
**/
void srandom(unsigned int seed) { OVERLOAD
	call("srandom(%u).", seed);
	um_srandom(seed);
}

/**
Generates the next pseudorandom number.

@return The number.
**/
long random() { OVERLOAD
	call("random().");
	return um_random();
}

short pairs = 0;

/**
Initializes a new color pair and tracks their amount.

@param pair The index of the pair.
@param f The foreground color.
@param b The background color.
@return Zero if no errors occurred and something else otherwise.
**/
int init_pair(short pair, short f, short b) { OVERLOAD
	call("init_pair(%d, %d, %d).", pair, f, b);
	pairs++;
	return um_init_pair(pair, f, b);
}

unsigned int previous_frame_count = 0;//or similar system to push keys into drawing queue

/**
Redraws the window.

Draws the custom interface.

@param win The window to redraw.
@return Zero if no errors occurred and something else otherwise.
**/
int wrefresh(WINDOW * win) { OVERLOAD
	call("wrefresh(0x%08x).", (unsigned int )win);

	/*
	Stores the state of the window.

	Pointers are used to suppress a warning about a bug in a library.
	<pre>
	the comparison will always evaluate as 'true' for the address of 'attrs' will never be NULL [-Waddress]
	</pre>
	*/
	int y, x;
	attr_t attrs; attr_t * _attrs = &attrs;
	short pair; short * _pair = &pair;
	getyx(win, y, x);
	wattr_get(win, _attrs, _pair, NULL);

	/*
	Initializes the color pairs used by the interface.
	*/
	short ws_pair = pairs;
	#define wrefresh_INIT_PAIR(b) \
		um_init_pair(ws_pair, COLOR_BLACK, b);\
		ws_pair++;
	wrefresh_INIT_PAIR(COLOR_RED);
	wrefresh_INIT_PAIR(COLOR_YELLOW);
	wrefresh_INIT_PAIR(COLOR_GREEN);
	wrefresh_INIT_PAIR(COLOR_CYAN);
	wrefresh_INIT_PAIR(COLOR_BLUE);
	wrefresh_INIT_PAIR(COLOR_MAGENTA);

	/*
	Draws the status bar.

	TODO make non-static with padding
	*/
	int ws_x = TERM_COL, ws_y = TERM_ROW-1;
	char ws_str[TERM_COL], ws_buf[TERM_COL];
	#define wrefresh_ADDSTR(format, length, ...) \
		ws_pair--;\
		wattrset(win, COLOR_PAIR(ws_pair));\
		snprintf(ws_str, (size_t )TERM_COL, format, __VA_ARGS__);\
		snprintf(ws_buf, (size_t )((condensed ? 1 : length)+1), "%-*s", length, ws_str);\
		ws_x -= length;\
		/*ws_x--;*/\
		mvaddnstr(ws_y, ws_x, ws_buf, TERM_COL-ws_x);\
		ws_x--;
	wrefresh_ADDSTR("S: %u/%u", 6, globstate, states-1);
	wrefresh_ADDSTR("D: %u", 13, (unsigned int )(current_time-record.time));
	wrefresh_ADDSTR("R: 0x%08x", 13, hash(ARC4_S, 0x100));
	wrefresh_ADDSTR("T: ?/%u", 13, TURNS);
	wrefresh_ADDSTR("F: ?/%u", 13, record.count);
	wrefresh_ADDSTR("I: %s", 9, codeins);

	/*
	Draws the debug bar.
	*/
	char some[TERM_COL];//a hack
	strcpy(some, "P:");
	for (int index = 0; index < states; index++) {
		if (shm->pids != NULL) {
			char somer[TERM_COL];
			bool somery = shm->pids[index] != 0;
			sprintf(somer, "%s %c%06d%c", some, somery ? '[' : ' ', (unsigned short )shm->pids[index], somery ? ']' : ' ');
			strcpy(some, somer);
		}
	}
	mvaddnstr(21, 10, some, TERM_COL-20);

	/*
	Tries something.
	*/
	/*WINDOW * subwin = newwin(1, 16, 8, 8);
	waddstr(subwin, "Hooray.");
	um_wrefresh(subwin);
	delwin(subwin);*/

	/*
	Restores the state of the window.
	*/
	wattr_set(win, attrs, pair, NULL);
	wmove(win, y, x);

	/*
	Redraws the window.
	*/
	return um_wrefresh(win);
}

/**
Reads a key code from a window.

@param win The window to read from.
@return The key code.
**/
int wgetch(WINDOW * win) { OVERLOAD//bloat
	call("wgetch(0x%08x).", (unsigned int )win);
	if (playbacking) {
		if (playback_frame != NULL) {//TODO move this
			if (playback_frame->duration == 0) {
				current_time += playback_frame->value;
				seed(current_time);
				playback_frame = playback_frame->next;
				return 0;
			}
			else {
				struct timespec req;
				bool out_of_variable_names = FALSE;
				if (playback_frame->duration >= fps) out_of_variable_names = TRUE;
				req.tv_sec = (time_t )(out_of_variable_names ? playback_frame->duration : 0);
				req.tv_nsec = out_of_variable_names ? 0l : 1000000000l/fps*playback_frame->duration;
				nanosleep(&req, NULL);
				const int yield = playback_frame->value;
				playback_frame = playback_frame->next;
				return yield;
			}
		}
	}
	int key = um_wgetch(win);
	if (key == KEY_F(8)) {
		if (record.count == 0) {//move to playback
			freadp(&record, input_file);
			playbacking = TRUE;
			playback_frame = record.first;
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
		globstate = globstate%(states-1)+1;//++
		//globstate = (globstate-2)%(states-1)+1;//--
		wrefresh(win);
		return 0;
	}
	else if (key == KEY_F(11)) {//changes the time
		current_time++;
		wrefresh(win);
		return 0;
	}
	else if (key == KEY_F(12)) {//saves the seed frame (later S)
		seed(current_time);
		add_seed_frame(&record, current_time);
		wrefresh(win);
		return 0;
	}
	else if (key == '_') {//dumps everything
		fwritep(&record, output_files[0]);//TODO move
		return 0;
	}
	else if (key == 'Q') {//quits everything (stupid idea or implementation)
		/*fprintfl(warning_stream, "[%06d::send(TERM)]", (unsigned short )getpid());
		for (int index = 0; index < states; index++) {
			if (shm->pids[index] != 0 && shm->pids[index] != getpid()) {
				kill(shm->pids[index], SIGTERM);
				shm->pids[index] = 0;
			}
		}*/
		kill(shm->ppid, SIGTERM);
		kill(getpid(), SIGTERM);
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
	unsigned char duration = fps/2;
	add_key_frame(&record, duration, key);//meta, colon and w are undisplayed but still recorded (for now)
	//wrefresh(win);
	return key;
}

#endif
