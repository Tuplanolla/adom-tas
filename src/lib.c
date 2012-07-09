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

#include <sys/wait.h>

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
#include "interface.h"

#include "lib.h"

void dlnull(void) {
	exit(error(ASSERT_PROBLEM));//TODO fix exit
}

intern printf_f um_printf = (printf_f )dlnull;
intern unlink_f um_unlink = (unlink_f )dlnull;
intern ioctl_f um_ioctl = (ioctl_f )dlnull;
intern time_f um_time = (time_f )dlnull;
intern localtime_f um_localtime = (localtime_f )dlnull;
intern srandom_f um_srandom = (srandom_f )dlnull;
intern random_f um_random = (random_f )dlnull;
intern init_pair_f um_init_pair = (init_pair_f )dlnull;
intern wrefresh_f um_wrefresh = (wrefresh_f )dlnull;
intern wgetch_f um_wgetch = (wgetch_f )dlnull;
intern endwin_f um_endwin = (endwin_f )dlnull;

/**
The active save state.
**/
unsigned int current_state = 1;

/**
The most important variables ever defined.
**/
bool was_meta = FALSE;//not good
int was_colon = FALSE;//worse
bool playbacking = FALSE;
bool rolling = FALSE;
frame_t * playback_frame;
bool running = TRUE;
char name[77];
unsigned char dur = 15;
int surplus_turns = 0;
int previous_turns = 0;

/**
Whether an overloaded function call is the first one.
**/
bool first = TRUE;

void * libc_handle;
void * libncurses_handle;

/*
Opens the dynamically linked libraries.
*/
problem_t init_lib(void) {
	/*
	Loads the unmodified functions.

	<code>RTLD_LAZY</code> is faster than <code>RTLD_NOW</code>.
	*/
	const int mode = RTLD_LAZY;

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
	if (um_printf == NULL
			|| um_unlink == NULL
			|| um_time == NULL
			|| um_localtime == NULL
			|| um_srandom == NULL
			|| um_random == NULL
			|| um_ioctl == NULL
			|| um_endwin == NULL) {
		return error(LIBC_DLSYM_PROBLEM);
	}

	libncurses_handle = dlopen(libncurses_path, mode);
	if (libncurses_handle == NULL) {
		return error(LIBNCURSES_DLOPEN_PROBLEM);
	}
	um_init_pair = (init_pair_f )dlsym(libncurses_handle, "init_pair");
	um_wrefresh = (wrefresh_f )dlsym(libncurses_handle, "wrefresh");
	um_wgetch = (wgetch_f )dlsym(libncurses_handle, "wgetch");
	um_endwin = (endwin_f )dlsym(libncurses_handle, "endwin");
	if (um_init_pair == NULL
			|| um_wrefresh == NULL
			|| um_wgetch == NULL) {
		return error(LIBNCURSES_DLSYM_PROBLEM);
	}

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

Intercepts printing anything and initializes this process.

@param format The string format.
@return The amount of characters printed.
**/
int printf(const char * format, ...) {
	if (first) {//TODO simplify
		first = FALSE;
		const problem_t problem = init_parent();
		if (problem != NO_PROBLEM) {
			uninit_parent(problem);
		}
		init_interface();
	}

	call("printf(...).");
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
Resizing the terminal causes spurious calls and prints garbage on the screen.

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

/**
Initializes a new color pair.

@param pair The index of the pair.
@param f The foreground color.
@param b The background color.
@return 0 if no errors occurred and -1 otherwise.
**/
int init_pair(short pair, short f, short b) {
	call("init_pair(%d, %d, %d).", pair, f, b);
	return um_init_pair(pair, f, b);
}

bool skipwr = FALSE;

/**
Redraws the window.

Draws the custom interface.

@param win The window to redraw.
@return 0 if no errors occurred and -1 otherwise.
**/
int wrefresh(WINDOW * win) {
	if (skipwr) return 0;
	call("wrefresh(0x%08x).", (unsigned int )win);

	/*
	Stores the state of the window,
	draws the interface
	and restores the state.

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
	draw_interface(win);
	wmove(win, y, x);
	wattr_set(win, attrs, pair, NULL);

	return um_wrefresh(win);
}

int previous_key = 0;

int rollstage = 0;

/**
Reads a key code from a window.

@param win The window to read from.
@return The key code.
**/
int wgetch(WINDOW * win) {//TODO remove bloat
	call("wgetch(0x%08x).", (unsigned int )win);

	#define ROLL_FOR_PLAYING FALSE
	if (rolling) {
		rollstage++;
		char str[22];
		switch (rollstage) {
			case -127-ROLL_FOR_PLAYING: {
				int * birthday = (int * )0x082b61f0;
				int * attributes = (int * )0x082b1728;
				int * items = (int * )0x082a5980;
				int * books = (int * )0x082a7e00;
				if (books[0x14] == 0
						|| books[0x1e] == 0
						|| items[0xa9] == 0
						|| attributes[0x01] < 20) exit(0);
				snprintf(str, 22, "chr/%u", (unsigned int )timestamp);
				FILE * const f = fopen(str, "w");
				if (f != NULL) {
					//fwrite(birthday, sizeof (int), 0x01, f);
					fwrite(attributes, sizeof (int), 0x09, f);
					fwrite(items, sizeof (int), 0x2b9, f);
					fwrite(books, sizeof (int), 0x2f, f);
					fclose(f);
					exit(0);
				}
				exit(1);
			}
			case 1: return 'g';
			case 2: return ' ';
			case 3: return 's';
			case 4: return 'm';
			case 5: return 'g';
			case 6: return 'f';
			case 7: return ' ';
			case 8: return 'q';
			//case 8: {rollstage = -128;return 'r';}//return 'q';
			default:
				mvinnstr(0, 0, str, 21);
				if (strncmp(str, "Your father wants you", 16) == 0) return 'd';
				else if (strncmp(str, "In your childhood sla", 16) == 0) return 'c';
				else if (strncmp(str, "You and your friends ", 16) == 0) return 'b';
				else if (strncmp(str, "In cold winters lots ", 16) == 0) return 'c';
				else if (strncmp(str, "Your grandfather alwa", 16) == 0) return 'a';
				else if (strncmp(str, "When exploring a long", 16) == 0) return 'a';
				else if (strncmp(str, "You are offered two j", 16) == 0) return 'a';
				else if (strncmp(str, "Your favorite teacher", 16) == 0) return 'd';
				else if (strncmp(str, "While crossing a ford", 16) == 0) return 'b';
				else if (strncmp(str, "You have been trainin", 16) == 0) return 'c';
				else if (strncmp(str, "At one point you fall", 16) == 0) return 'd';
				else if (strncmp(str, "While at a market, yo", 16) == 0) return 'c';
				else if (strncmp(str, "While playing in the ", 16) == 0) return 'b';
				else if (strncmp(str, "Soon after starting y", 16) == 0) return 'a';
				else if (strncmp(str, "After serving your ma", 16) == 0) return 'd';
				else if (strncmp(str, "Having finally master", 16) == 0) return 'a';
				else if (strncmp(str, "The final day of your", 16) == 0) return 'd';
				else if (strncmp(str, "After ending your app", 16) == 0) return 'c';
				else if (strncmp(str, "Evil has started to i", 16) == 0) return 'c';
				else if (strncmp(str, "Now that you are old ", 16) == 0) return 'b';
				else if (strncmp(str, "As you make your way ", 16) == 0) return 'd';
				else if (strncmp(str, "During your apprentic", 16) == 0) return 'b';
				else if (strncmp(str, "While doing research ", 16) == 0) return 'a';
				else if (strncmp(str, "During a night out, o", 16) == 0) return 'a';
				else if (strncmp(str, "While on a journey, y", 16) == 0) return 'a';
				else if (strncmp(str, "If you came across an", 16) == 0) return 'd';
				else if (strncmp(str, "Your brother has come", 16) == 0) return 'b';
				else if (strncmp(str, "You are carrying your", 16) == 0) return 'b';
				else if (strncmp(str, "You have stolen to fe", 16) == 0) return 'a';
				else if (strncmp(str, "Your king lies dying,", 16) == 0) return 'a';
				else if (strncmp(str, "You are called home t", 16) == 0) return 'c';
				else if (strncmp(str, "You are a witness to ", 16) == 0) return 'a';
				else if (strncmp(str, "As a child, whenever ", 16) == 0) return 'b';
				else if (strncmp(str, "While shopping, you s", 16) == 0) return 'a';
				else if (strncmp(str, "One night you wake up", 16) == 0) return 'b';
				else if (strncmp(str, "When your father took", 16) == 0) return 'b';
				else if (strncmp(str, "What would you order ", 16) == 0) return 'b';
				else if (strncmp(str, "What about you gave y", 16) == 0) return 'b';
				else if (strncmp(str, "Will you fight evil..", 16) == 0) return 'b';
				else if (strncmp(str, "You are in a nobleman", 16) == 0) return 'b';
				else if (strncmp(str, "There have been whisp", 16) == 0) return 'b';
				else if (strncmp(str, "Your mind made up, yo", 16) == 0) return 'b';
				else if (strncmp(str, "On your way to the Dr", 16) == 0) return 'c';
				else if (strncmp(str, "You attempt to make a", 16) == 0) return 'd';
				else if (strncmp(str, "You are walking on th", 16) == 0) return 'd';
				else if (strncmp(str, "You are chopping wood", 16) == 0) return 'd';
				else if (strncmp(str, "You return back from ", 16) == 0) return 'b';
				else if (strncmp(str, "Your father has appre", 16) == 0) return 'd';
				else if (strncmp(str, "You find yourself wor", 16) == 0) return 'b';
				else if (strncmp(str, "While learning tricks", 16) == 0) return 'c';
				else if (strncmp(str, "After many years of t", 16) == 0) return 'd';
				else rollstage = -128; break;
		}
		return 0;
	}

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

	if (*executable_turns < previous_turns) surplus_turns++;
	previous_turns = *executable_turns;
	int key = um_wgetch(win);
	if (key == play_key) {
		if (record.count == 1) {//move to roll
			rolling = TRUE;
			back: timestamp++;
			struct tm * tm;
			tm = gmtime(&timestamp);
			if (!(tm->tm_mon == 11 && tm->tm_mday == 31)) {
				tm->tm_sec = 0;
				tm->tm_min = 0;
				tm->tm_hour = 0;
				tm->tm_mday = 31;
				tm->tm_mon = 11;
				tm->tm_isdst = 0;
				timestamp = mktime(tm) - timezone;
			}
			iarc4((unsigned int )timestamp, executable_arc4_calls_menu);
			if (fork() > 0) {
				int s;
				wait(&s);
				if (s == 0) goto back;
			}
			else skipwr = TRUE;
			return 0;
		}
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
		MODINC(current_state, states);
		wrefresh(win);
		return 0;
	}
	else if (key == unstate_key) {
		MODDEC(current_state, states);
		wrefresh(win);
		return 0;
	}
	else if (key == menu_key) {
		/*if (inactive) {
			redrawwin(win);
			um_wrefresh(win);
		}*/
		inactive = !inactive;
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
		if (dur < 255) dur = (unsigned char )((dur + 1) * 2 - 1);
		wrefresh(win);
		return 0;
	}
	else if (key == unduration_key) {
		if (dur > 0) dur = (unsigned char )((dur + 1) / 2 - 1);
		wrefresh(win);
		return 0;
	}
	else if (key == quit_key) {//quits everything (stupid implementation)
		running = FALSE;
		um_endwin();
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
	const char * code = key_code(key);
	strcpy(name, "");
	strcat(name, code);
	add_key_frame(dur, key);
	wrefresh(win);
	previous_key = key;
	return key;
}

/**
Ends drawing to the screen.

Intercepts exiting prematurely.
Currently the process jams.

@return <code>OK</code> if successful and <code>ERR</code> otherwise.
**/
int _endwin(void) {
	call("endwin().");
	while (running) {
		wgetch(stdscr);
	}
	curs_set(1);
	return um_endwin();
}

#endif
