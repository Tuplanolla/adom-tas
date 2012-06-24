/**
Provides reading for the whole family.
**/
#ifndef LIB_C
#define LIB_C

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
#include "adom.h"
#include "shm.h"
#include "problem.h"
#include "record.h"
#include "lib.h"
#include "loader.h"

UNLINK um_unlink = NULL;
IOCTL um_ioctl = NULL;
TIME um_time = NULL;
LOCALTIME um_localtime = NULL;
SRANDOM um_srandom = NULL;
RANDOM um_random = NULL;
INIT_PAIR um_init_pair = NULL;
WREFRESH um_wrefresh = NULL;
WGETCH um_wgetch = NULL;
EXIT um_exit = NULL;

char * home_path;
char * executable_path;
char * executable_data_path;
char * executable_process_path;
char * executable_version_path;
char * executable_count_path;
char * executable_keybind_path;
char * executable_config_path;
char * loader_path;
char * libc_path;
char * libncurses_path;
unsigned int generations;
unsigned int states;
unsigned int rows;
unsigned int cols;
char * iterator;
FILE * input_stream;
FILE ** output_streams;
char * shm_path;
FILE * error_stream;
FILE * warning_stream;
FILE * note_stream;
FILE * call_stream;
record_t record;

bool initialized = FALSE;

/**
Annotates and initializes overloaded functions.
**/
#define OVERLOAD if (!initialized) initialized = init_parent() == NO_PROBLEM;

/**
Very important temporary variables.
**/
record_t record;
int globstate = 1;
time_t current_time = 0;//0x7fe81780

/**
Redirects calls from injected instructions.
**/
void seed(const int seed) {
	iarc4(seed, executable_arc4_calls_automatic_load);
}

void injector(void) {
	seed(current_time);
	add_seed_frame(&record, current_time);
	wrefresh(stdscr);
}

bool was_meta = FALSE;//not good
int was_colon = FALSE;//worse
bool condensed = FALSE;
bool playbacking = FALSE;
frame_t * playback_frame;
char name[7];

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
long random(void) { OVERLOAD
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
	const int TERM_COL = 77;
	const int TERM_ROW = 25;
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
	wrefresh_ADDSTR("R: 0x%08x", 13, hash(executable_arc4_s, 0x100));
	wrefresh_ADDSTR("T: ?/%u", 13, *executable_turns);
	wrefresh_ADDSTR("F: ?/%u", 13, record.count);
	wrefresh_ADDSTR("I: %s", 9, name);

	/*
	Draws the debug bar.
	*/
	char some[TERM_COL];//a hack
	strcpy(some, "P:");
	for (int index = 0; index < states; index++) {
		if (shm->pids != NULL) {
			char somer[TERM_COL];
			bool somery = shm->pids[index] != 0;
			sprintf(somer, "%s %c%d%c", some, somery ? '[' : ' ', (unsigned short )shm->pids[index], somery ? ']' : ' ');
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
				if (playback_frame->duration >= frame_rate) out_of_variable_names = TRUE;
				req.tv_sec = (time_t )(out_of_variable_names ? playback_frame->duration : 0);
				req.tv_nsec = out_of_variable_names ? 0l : 1000000000l/frame_rate*playback_frame->duration;
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
			freadp(input_stream, &record);
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
	else if (key == '_') {//dumps everything
		fwritep(output_streams[0], &record);//TODO move
		return 0;
	}
	else if (key == 'Q') {//quits everything (stupid idea or implementation)
		/*fprintfl(warning_stream, "[%d::send(TERM)]", (unsigned short )getpid());
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
		strcpy(name, "");
		if (was_colon) {
			key_code(code, was_colon == 1 ? 0x3a : 'w');
			strcat(name, code);
		}
		if (was_meta) {
			key_code(code, 0x1b);
			strcat(name, code);
		}
		was_colon = FALSE;
		was_meta = FALSE;
		key_code(code, key);
		strcat(name, code);//TODO turn this into a macro
	}
	unsigned char duration = frame_rate/2;
	add_key_frame(&record, duration, key);//meta, colon and w are undisplayed but still recorded (for now)
	//wrefresh(win);
	return key;
}

/**
Exits immediately.

Intercepts exiting prematurely.

@param status The return value.
**/
void exit(int status) { OVERLOAD
	call("exit(%d).", status);
	//do something
	um_exit(NO_PROBLEM);
}

#endif
