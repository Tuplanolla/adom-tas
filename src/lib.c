/**
Does something important.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef LIB_C
#define LIB_C

#include <stdarg.h>//va_*
#include <stdlib.h>//*env, NULL
#include <stdio.h>//*open, *close, *read, *write, FILE
#include <string.h>//str*, mem*
#include <unistd.h>//sleep, temporary kill
#include <time.h>//time_t
#include <signal.h>//temporary SIGKILL
#include <dlfcn.h>//dl*, RTLD_*
#include <sys/ioctl.h>//TIOC*

#include <curses.h>//*w*, chtype, WINDOW, COLOR

#include "util.h"
#include "exec.h"
#include "shm.h"
#include "problem.h"
#include "record.h"
#include "put.h"
#include "loader.h"
#include "fork.h"
#include "log.h"
#include "config.h"

#include "lib.h"

intern printf_f um_printf = NULL;
intern unlink_f um_unlink = NULL;
intern ioctl_f um_ioctl = NULL;
intern time_f um_time = NULL;
intern localtime_f um_localtime = NULL;
intern srandom_f um_srandom = NULL;
intern random_f um_random = NULL;
intern init_pair_f um_init_pair = NULL;
intern wrefresh_f um_wrefresh = NULL;
intern wgetch_f um_wgetch = NULL;
intern exit_f um_exit = NULL;

/**
The active save state.
**/
unsigned int current_state = 1;

bool was_meta = FALSE;//not good
int was_colon = FALSE;//worse
bool condensed = FALSE;
bool playbacking = FALSE;
frame_t * playback_frame;
char name[7];
unsigned char dur = 16;

/**
Whether an overloaded function call is the first one.
**/
bool first = TRUE;

void * libc_handle;
void * libncurses_handle;

problem_t init_lib(void) {
	/*
	Loads functions from dynamically linked libraries.

	Requires either <code>RTLD_LAZY</code> or <code>RTLD_NOW</code>.
	*/
	const int mode = RTLD_NOW;

	libc_handle = dlopen(libc_path, mode);
	if (libc_handle == NULL) {
		return error(LIBC_DLOPEN_PROBLEM);
	}
	um_printf = (printf_f )dlsym(libc_handle, "printf");
	um_unlink = (unlink_f )dlsym(libc_handle, "unlink");
	um_time = (time_f )dlsym(libc_handle, "time");
	um_localtime = (localtime_f )dlsym(libc_handle, "localtime");
	um_srandom = (srandom_f )dlsym(libc_handle, "srandom");
	um_random = (random_f )dlsym(libc_handle, "random");
	um_ioctl = (ioctl_f )dlsym(libc_handle, "ioctl");
	um_exit = (exit_f )dlsym(libc_handle, "exit");

	libncurses_handle = dlopen(libncurses_path, mode);
	if (libncurses_handle == NULL) {
		return error(LIBNCURSES_DLOPEN_PROBLEM);
	}
	um_init_pair = (init_pair_f )dlsym(libncurses_handle, "init_pair");
	um_wrefresh = (wrefresh_f )dlsym(libncurses_handle, "wrefresh");
	um_wgetch = (wgetch_f )dlsym(libncurses_handle, "wgetch");

	/*
	Prevents reloading libraries for child processes.
	*/
	if (unsetenv("LD_PRELOAD") == -1) {
		warning(LD_PRELOAD_UNSETENV_PROBLEM);
	}

	return NO_PROBLEM;
}

/*
Closes the dynamically linked libraries.
*/
problem_t uninit_lib(void) {
	if (dlclose(libc_handle) != 0) {
		return error(LIBC_DLCLOSE_PROBLEM);
	}
	if (dlclose(libncurses_handle) != 0) {
		return error(LIBNCURSES_DLCLOSE_PROBLEM);
	}

	return NO_PROBLEM;
}

/**
Emulates the process of saving, quitting and loading.
**/
void save_quit_load(void) {
	iarc4((unsigned int )timestamp, executable_arc4_calls_automatic_load);
	add_seed_frame(timestamp);
	wrefresh(stdscr);
}

/**
Prints a formatted string.

Intercepts printing anything.

@param format The string format.
@return The amount of characters printed.
**/
int printf(const char * format, ...) {
	if (first) {
		first = FALSE;
		const problem_t problem = init_parent();
		if (problem != NO_PROBLEM) {
			uninit_parent(problem);
		}
	}

	call("printf(\"%s\", ...).", format);
	return (int )strlen(format);//approximate
}

/**
Removes a file.

Intercepts removing the debug file if it exists.

@param path The path of the file to remove.
@return 0 if no errors occurred and -1 otherwise.
**/
int unlink(const char * path) {
	call("unlink(\"%s\").", path);
	if (strcmp(path, "ADOM.DBG") == 0) {
		sleep(1);
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
@return 0 if no errors occurred and -1 otherwise.
**/
int ioctl(int d, unsigned long request, ...) {
	va_list	argp;
	va_start(argp, request);
	void * arg = va_arg(argp, void *);
	call("ioctl(0x%08x, 0x%08x, 0x%08x).", (unsigned int )d, (unsigned int )request, (unsigned int )arg);
	const int result = um_ioctl(d, request, arg);
	if (request == TIOCGWINSZ) {
		struct winsize * size = (struct winsize * )arg;
		size->ws_row = (unsigned short )rows;
		size->ws_col = (unsigned short )cols;
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
time_t time(time_t * t) {
	call("time(0x%08x).", (unsigned int )t);
	if (t != NULL) *t = timestamp;
	return timestamp;//reduces entropy
}

/**
Converts a <code>time_t</code> to a broken-down <code>struct tm</code>.

Replaces <code>localtime</code> with <code>gmtime</code> to disregard timezones.

@param timep The <code>time_t</code> to convert.
@return The <code>struct tm</code>.
**/
struct tm * localtime(const time_t * timep) {
	call("localtime(0x%08x).", (unsigned int )timep);
	return gmtime(timep);//reduces entropy
}

/**
Seeds the pseudorandom number generator.

@param seed The seed.
**/
void srandom(unsigned int seed) {
	call("srandom(%u).", seed);
	um_srandom(seed);
}

/**
Generates the next pseudorandom number.

@return The number.
**/
long random(void) {
	call("random().");
	return um_random();
}

unsigned int pairs = 0;

/**
Initializes a new color pair and tracks their amount.

@param pair The index of the pair.
@param f The foreground color.
@param b The background color.
@return 0 if no errors occurred and -1 otherwise.
**/
int init_pair(short pair, short f, short b) {
	call("init_pair(%d, %d, %d).", pair, f, b);
	pairs++;
	return um_init_pair(pair, f, b);
}

unsigned int previous_frame_count = 0;//or similar system to push keys into drawing queue

/**
Redraws the window.

Draws the custom interface.

@param win The window to redraw.
@return 0 if no errors occurred and -1 otherwise.
**/
int previous_turns;
int wrefresh(WINDOW * win) {
	call("wrefresh(0x%08x).", (unsigned int )win);

	if (*executable_turns < previous_turns) turns++;
	previous_turns = *executable_turns;

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
	wattr_get(win, _attrs, _pair, NULL);
	getyx(win, y, x);

	wattrset(win, A_NORMAL);

	/*
	Initializes the color pairs used by the interface.
	*/
	short ws_pair = (short )pairs;
	#define wrefresh_INIT_PAIR(b) \
		um_init_pair(ws_pair, COLOR_BLACK, b);\
		ws_pair++;
	wrefresh_INIT_PAIR(COLOR_RED);
	wrefresh_INIT_PAIR(COLOR_YELLOW);
	wrefresh_INIT_PAIR(COLOR_GREEN);
	wrefresh_INIT_PAIR(COLOR_CYAN);
	wrefresh_INIT_PAIR(COLOR_BLUE);
	wrefresh_INIT_PAIR(COLOR_MAGENTA);
	wrefresh_INIT_PAIR(COLOR_RED);//again
	wrefresh_INIT_PAIR(COLOR_YELLOW);//again

	/*
	Draws the status bar.

	TODO make non-static with padding
	*/
	#define TERM_COL 77
	#define TERM_ROW 25
	int ws_x = TERM_COL, ws_y = TERM_ROW - 1;
	char ws_str[TERM_COL], ws_buf[TERM_COL];
	#define wrefresh_ADDSTR(format, length, ...) \
		ws_pair--;\
		wattrset(win, COLOR_PAIR(ws_pair));\
		snprintf(ws_str, (size_t )TERM_COL, format, ##__VA_ARGS__);\
		snprintf(ws_buf, (size_t )((condensed ? 1 : length) + 1), "%-*s", length, ws_str);\
		ws_x -= length;\
		/*ws_x--;*/\
		mvwaddnstr(win, ws_y, ws_x, ws_buf, TERM_COL-ws_x);\
		ws_x--;
	wrefresh_ADDSTR("P: #%u", 9, (unsigned int )getpid());
	wrefresh_ADDSTR("S: %u/%u", 8, current_state, states - 1);
	wrefresh_ADDSTR("V: +%u", 8, (unsigned int )(timestamp - record.timestamp));
	wrefresh_ADDSTR("R: 0x%08x", 13, hash(executable_arc4_s, 0x100));
	wrefresh_ADDSTR("D: %u", 8, dur);
	wrefresh_ADDSTR("T: 0/%u", 8, *executable_turns + turns);
	wrefresh_ADDSTR("F: 1/%u", 8, record.count);
	wrefresh_ADDSTR("I: %s", 8, name);

	/*
	Draws the debug bar.
	*/
	/*char some[TERM_COL];//a hack
	strcpy(some, "P:");
	for (unsigned int indecks = 0; indecks < states; indecks++) {
		char somer[TERM_COL];
		bool somery = shm.pids[indecks] != 0;
		sprintf(somer, "%s %c%d%c", some, somery ? '[' : ' ', (unsigned short )shm.pids[indecks], somery ? ']' : ' ');
		strcpy(some, somer);
	}
	mvwaddnstr(win, 21, 10, some, TERM_COL-20);*/

	/*
	Restores the state of the window.
	*/
	wmove(win, y, x);
	wattr_set(win, attrs, pair, NULL);

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
int wgetch(WINDOW * win) {//TODO remove bloat
	call("wgetch(0x%08x).", (unsigned int )win);
	if (playbacking) {
		if (playback_frame != NULL) {//TODO move this
			if (playback_frame->duration == 0) {
				timestamp += playback_frame->value;
				iarc4((unsigned int )timestamp, executable_arc4_calls_automatic_load);
				playback_frame = playback_frame->next;
				return 0;
			}
			else {
				struct timespec req;
				bool out_of_variable_names = FALSE;
				if (playback_frame->duration >= frame_rate) out_of_variable_names = TRUE;
				req.tv_sec = (time_t )(out_of_variable_names ? playback_frame->duration : 0);
				req.tv_nsec = out_of_variable_names ? 0l : 1000000000l / frame_rate * playback_frame->duration;
				nanosleep(&req, NULL);//TODO use a better timer
				const int yield = playback_frame->value;
				playback_frame = playback_frame->next;
				return yield;
			}
		}
	}
	int key = um_wgetch(win);
	if (key == play_key) {
		if (record.count == 0) {//move to playback
			freadp(input_path);
			playbacking = TRUE;
			playback_frame = record.first;
		}
		else condensed = !condensed;
		wrefresh(win);
		return 0;
	}
	else if (key == save_key) {//saves
		fwritep(output_paths[current_state]);
		save(current_state);
		wrefresh(win);
		return 0;
	}
	else if (key == load_key) {//loads
		load(current_state);
		wrefresh(win);
		return 0;//redundant
	}
	else if (key == state_key) {
		current_state = current_state%(unsigned int )((int )states-1)+1;//++
		wrefresh(win);
		return 0;
	}
	else if (key == unstate_key) {
		current_state = (unsigned int )(((int )current_state-2)%((int )states-1))+1;//--
		wrefresh(win);
		return 0;
	}
	else if (key == menu_key) {
		int y, x;
		getyx(win, y, x);
		WINDOW * subwin = newwin(rows / 2 + rows % 2 + 2, cols / 2 + cols % 2 + 2, rows / 4 - 2, cols / 4 - 1);
		chtype ch = (chtype )' ' | COLOR_PAIR(37);
		wborder(subwin, ch, ch, ch, ch, ch, ch, ch, ch);
		um_wrefresh(subwin);
		delwin(subwin);
		subwin = newwin(3, cols / 2 + cols % 2 + 2, rows * 3 / 4 + rows % 2 - 1, cols / 4 - 1);
		ch = (chtype )' ' | COLOR_PAIR(37);
		wborder(subwin, ch, ch, ch, ch, ch, ch, ch, ch);
		mvwhline(subwin, 1, 1, ch, cols / 2 + cols % 2);
		wattrset(subwin, COLOR_PAIR(37));
		unsigned int ws_x = 1;
		size_t len = strlen("<");
		mvwaddnstr(subwin, 1, ws_x, "<", len);
		ws_x += len + 1;
		unsigned int state = 1;
		while (ws_x < cols / 2 + cols % 2 - 2 && state < states) {
			len = uintlen(state) + 2;
			char * buf = malloc(len + 1);
			const bool occupied = shm.pids[state] != 0;
			snprintf(buf, len + 1, "%c%u%c", occupied ? '[' : ' ', state, occupied ? ']' : ' ');
			mvwaddnstr(subwin, 1, ws_x, buf, len);
			free(buf);
			const char * indicator = "^";
			if (state == current_state) {//TODO intlen / 2 as well
				mvwaddnstr(subwin, 2, ws_x + 1 - strlen(indicator) / 2, indicator, strlen(indicator));
			}
			state++;
			ws_x += len + 1;
		}
		len = strlen(">");
		mvwaddnstr(subwin, 1, cols / 2 + cols % 2 + 1 - len, ">", len);
		wattrset(subwin, A_NORMAL);
		um_wrefresh(subwin);
		delwin(subwin);
		for (unsigned int row = 0; row < rows; row += 2) {
			for (unsigned int col = 0; col < cols; col += 2) {
				mvwaddch(win, row / 2 + rows / 4 - 1, col / 2 + cols / 4, shm.chs[current_state][row][col]);
			}
		}
		wmove(win, y, x);
		wrefresh(win);
		return 0;
	}
	else if (key == time_key) {
		timestamp++;
		wrefresh(win);
		return 0;
	}
	else if (key == untime_key) {
		timestamp--;
		wrefresh(win);
		return 0;
	}
	else if (key == duration_key) {
		if (dur < 128) dur = (unsigned char )dur * 2;
		wrefresh(win);
		return 0;
	}
	else if (key == unduration_key) {
		if (dur > 1) dur = (unsigned char )dur / 2;
		wrefresh(win);
		return 0;
	}
	else if (key == quit_key) {//quits everything (stupid implementation)
		endwin();
		printf("Ctrl C will get you back to your beloved terminal if nothing else works.\n"); fflush(stdout);
		for (unsigned int state = 1; state < states; state++) {
			if (shm.pids[state] != 0) {
				kill(shm.pids[state], SIGKILL);
				shm.pids[state] = 0;
			}
		}
		kill(shm.ppid[0], SIGKILL);
		kill(shm.pids[0], SIGKILL);
		return 0;//nice and elegant
	}
	if (!was_meta && !was_colon && (key == 0x3a || key == 'w')) was_colon = key == 0x3a ? 1 : 2;//booleans are fun like that
	else if (!was_meta && key == 0x1b) was_meta = TRUE;
	else {
		const char * code;
		strcpy(name, "");
		if (was_colon) {
			code = key_code(was_colon == 1 ? 0x3a : 'w');
			strcat(name, code);
		}
		if (was_meta) {
			code = key_code(0x1b);
			strcat(name, code);
		}
		was_colon = FALSE;
		was_meta = FALSE;
		code = key_code(key);
		strcat(name, code);//TODO turn this into a macro
	}
	add_key_frame(dur, key);//meta, colon and w are undisplayed but still recorded (for now)
	//wrefresh(win);
	return key;
}

/**
Exits immediately.

Intercepts exiting prematurely.
Currently keys are lost and the process jams.
Intercepting endwin with a wgetch loop should fix the problem.

@param status The return value.
**/
void exit(int status) {
	call("exit(%d).", status);
	/*while (TRUE) {
		sleep(1);
	}*/
	um_exit(NO_PROBLEM);
}

#endif
