/**
Does something unnecessary.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef LIB_C
#define LIB_C

#include <stdarg.h>//va_*, exit
#include <stdlib.h>//*env, NULL
#include <stdio.h>//*open, *close, *read, *write, FILE
#include <string.h>//str*, mem*
#include <unistd.h>//fork, sleep, temporary kill
#include <time.h>//time_t
#include <signal.h>//sig*, SIG*
#include <dlfcn.h>//dl*, RTLD_*
#include <sys/ioctl.h>//TIOC*
#include <sys/stat.h>//stat

#include <sys/wait.h>

#include <curses.h>//*w*, chtype, WINDOW, COLOR

#include "prob.h"//problem_d, PROPAGATE*
#include "log.h"//error, warning, note
#include "cfg.h"//*
#include "asm.h"//inject_*
#include "shm.h"//*_shm, shm
#include "rec.h"//record

#include "util.h"
#include "exec.h"
#include "shm.h"
#include "prob.h"
#include "rec.h"
#include "put.h"
#include "lib.h"
#include "fork.h"
#include "log.h"
#include "cfg.h"
#include "gui.h"

#include "lib.h"

/**
The states of this module.
**/
enum fcn_state_e {
	NOT_INITIALIZED,
	SOMEWHAT_INITIALIZED,
	FULLY_INITIALIZED
};
typedef enum fcn_state_e fcn_state_d;

/**
The state of this module.
**/
fcn_state_d fcn_state = NOT_INITIALIZED;

/**
The most important variable ever defined.
**/
frame_d * current_frame = NULL;
unsigned char current_duration = 15;
int surplus_turns = 0;
int previous_turns = 0;
/**
The amount of frames at the previous input.
**/
intern unsigned int previous_count = 0;

/**
The active save state.
**/
intern int current_state = 1;
/**
The four last inputs.
**/
intern int previous_inputs[4];
/**
Whether a menu is open and inputs are blocked.
**/
intern bool inactive = FALSE;
/**
Whether the interface is condensed.
**/
intern bool condensed = FALSE;
/**
Whether the interface is hidden entirely.
**/
intern bool hidden = FALSE;
/**
Whether a record is being played back.
**/
intern bool playing = FALSE;
/**
Whether a record is paused.
**/
intern bool paused = FALSE;
/**
Whether this module is exiting.
**/
intern bool quitting = FALSE;
/**
Whether characters are being rolled.
**/
intern bool rolling = FALSE;

/**
Some dlfcn stuff.
**/
void * libc_handle;
void * libncurses_handle;

/**
Emulates the process of saving, quitting and loading.
**/
void save_quit_load(void) {
	/*
	The only reliable case (automatic loading) is assumed.

	Manual loading changes the random number generator's state depending on
		the available save games and
		actions in the menu.
	*/
	iarc4((unsigned int )timestamp, exec_arc4_calls_automatic_load);
	(*exec_saves)++;
	add_seed_frame(timestamp);
	wrefresh(stdscr);
}

/**
Uninitializes this process.
**/
problem_d uninit_common(void) {
	PROPAGATE(detach_shm());

	PROPAGATE(uninit_config());

	return NO_PROBLEM;
}

/**
Uninitializes this process.

@param problem The error code to return.
**/
problem_d uninit_child(const problem_d problem) {
	//PROPAGATE(uninit_fcn());

	/*
	Exits gracefully.
	*/
	exit(problem);

	return NO_PROBLEM;
}

/**
Uninitializes all processes.

@param problem The error code to return.
**/
problem_d uninit_parent(const problem_d problem) {
	if (dlclose(libc_handle) != 0) {
		return error(LIBC_DLCLOSE_PROBLEM);
	}
	if (dlclose(libncurses_handle) != 0) {
		return error(LIBNCURSES_DLCLOSE_PROBLEM);
	}

	PROPAGATE(uninit_shm());

	/*
	Exits gracefully.
	*/
	exit(problem);

	return NO_PROBLEM;
}

void dlnull(void) {
	uninit_parent(error(ASSERT_PROBLEM));
}

intern printf_f um_printf = (printf_f )dlnull;
intern unlink_f um_unlink = (unlink_f )dlnull;
intern ioctl_f um_ioctl = (ioctl_f )dlnull;
intern time_f um_time = (time_f )dlnull;
intern localtime_f um_localtime = (localtime_f )dlnull;
intern srandom_f um_srandom = (srandom_f )dlnull;
intern random_f um_random = (random_f )dlnull;
intern wrefresh_f um_wrefresh = (wrefresh_f )dlnull;
intern init_pair_f um_init_pair = (init_pair_f )dlnull;
intern waddnstr_f um_waddnstr = (waddnstr_f )dlnull;
intern wgetch_f um_wgetch = (wgetch_f )dlnull;
intern endwin_f um_endwin = (endwin_f )dlnull;

/**
Initializes this process.
**/
problem_d init_parent(void) {
	PROPAGATE(init_internal_config());

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
	um_ioctl = (ioctl_f )dlsym(libc_handle, "ioctl");
	um_time = (time_f )dlsym(libc_handle, "time");
	um_localtime = (localtime_f )dlsym(libc_handle, "localtime");
	um_srandom = (srandom_f )dlsym(libc_handle, "srandom");
	um_random = (random_f )dlsym(libc_handle, "random");
	if (um_printf == NULL
			|| um_unlink == NULL
			|| um_time == NULL
			|| um_localtime == NULL
			|| um_srandom == NULL
			|| um_random == NULL
			|| um_ioctl == NULL) {
		return error(LIBC_DLSYM_PROBLEM);
	}

	libncurses_handle = dlopen(libncurses_path, mode);
	if (libncurses_handle == NULL) {
		return error(LIBNCURSES_DLOPEN_PROBLEM);
	}
	um_wrefresh = (wrefresh_f )dlsym(libncurses_handle, "wrefresh");
	um_init_pair = (init_pair_f )dlsym(libncurses_handle, "init_pair");
	um_waddnstr = (waddnstr_f )dlsym(libncurses_handle, "waddnstr");
	um_wgetch = (wgetch_f )dlsym(libncurses_handle, "wgetch");
	um_endwin = (endwin_f )dlsym(libncurses_handle, "endwin");
	if (um_init_pair == NULL
			|| um_wrefresh == NULL
			|| um_wgetch == NULL
			|| um_endwin == NULL) {
		return error(LIBNCURSES_DLSYM_PROBLEM);
	}

	/*
	Prevents reloading libraries for child processes.
	*/
	if (unsetenv("LD_PRELOAD") == -1) {
		warning(LD_PRELOAD_UNSETENV_PROBLEM);
	}

	//PROPAGATE(init_interface());//TODO move

	/*
	Enables save-quit-load emulation.
	*/
	if (sql_emulation) {
		inject_save(&save_quit_load);
	}
	else {
		//inject_save(&nothing);
	}

	/*
	Initializes the shared memory segment.
	*/
	PROPAGATE(init_shm());

	record.timestamp = timestamp;

	const pid_t pid = 0;//fork();//disabled to make debugging easier
	if (pid == -1) {
		return error(FORK_PROBLEM);
	}
	else if (pid == 0) {//child
		PROPAGATE(attach_shm());
		shm.ppid[0] = getppid();
		shm.pids[0] = getpid();
	}
	else {//parent
		signal(SIGCHLD, SIG_IGN);

		sigset_t mask;
		sigfillset(&mask);
		sigdelset(&mask, SIGINT);//temporary
		sigdelset(&mask, SIGTERM);
		sigsuspend(&mask);

		PROPAGATE(uninit_parent(NO_PROBLEM));
	}

	return NO_PROBLEM;
}

/**
Prints a formatted string.

Intercepts printing anything and initializes this process.

@param format The string format.
@return The amount of characters printed.
**/
int printf(const char * const format, ...) {
	if (fcn_state == NOT_INITIALIZED) {
		fcn_state = SOMEWHAT_INITIALIZED;
		PROPAGATEF(init_parent(), uninit_parent);
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
int unlink(const char * const path) {
	call("unlink(\"%s\").", path);
	if (strcmp(path, "ADOM.DBG") == 0) {
		struct stat buf;
		if (stat(path, &buf) == 0) {
			sleep(1);
			return 0;
		}
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
int ioctl(const int d, const unsigned long request, ...) {
	va_list	argp;
	va_start(argp, request);
	void * arg = va_arg(argp, void *);
	call("ioctl(0x%08x, 0x%08x, 0x%08x).", (unsigned int )d, (unsigned int )request, (unsigned int )arg);
	const int result = um_ioctl(d, request, arg);
	if (request == TIOCGWINSZ) {
		struct winsize * size = (struct winsize * )arg;
		size->ws_row = (unsigned short int )rows;
		size->ws_col = (unsigned short int )cols;
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
time_t time(time_t * const t) {
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
struct tm * localtime(const time_t * const timep) {
	call("localtime(0x%08x).", (unsigned int )timep);
	return gmtime(timep);//reduces entropy
}

/**
Seeds the pseudorandom number generator.

@param seed The seed.
**/
void srandom(const unsigned int seed) {
	call("srandom(%u).", seed);
	um_srandom(seed);
}

/**
Generates the next pseudorandom number.

@return The number.
**/
long int random(void) {
	call("random().");
	return um_random();
}

bool skipwr = FALSE;

/**
Redraws the window.

Draws the custom interface.

@param win The window to redraw.
@return 0 if no errors occurred and -1 otherwise.
**/
int wrefresh(WINDOW * const win) {
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
	short int pair; short int * _pair = &pair;
	wattr_get(win, _attrs, _pair, NULL);
	getyx(win, y, x);
	wattrset(win, A_NORMAL);
	const int result = um_wrefresh(win);
	draw_interface();
	wmove(win, y, x);
	wattr_set(win, attrs, pair, NULL);

	return result;
}

/**
Initializes a new color pair.

@param pair The index of the pair.
@param f The foreground color.
@param b The background color.
@return 0 if no errors occurred and -1 otherwise.
**/
int init_pair(const short int pair, const short int f, const short int b) {
	call("init_pair(%d, %d, %d).", pair, f, b);
	pairs++;
	return um_init_pair(pair, f, b);
}

/**
Redraws the window.

Draws the custom interface.

@param win The window to redraw.
@return 0 if no errors occurred and -1 otherwise.
**/
int waddnstr(WINDOW * const win, const char * const str, const int n) {
	if (fcn_state == SOMEWHAT_INITIALIZED) {
		fcn_state = FULLY_INITIALIZED;
		PROPAGATEF(init_interface(), uninit_parent);
	}

	return um_waddnstr(win, str, n);
}

int previous_key = 0;

/**
Reads a key code from a window.

@param win The window to read from.
@return The key code.
**/
int wgetch(WINDOW * const win) {//TODO remove bloat and refactor with extreme force
	call("wgetch(0x%08x).", (unsigned int )win);

	if (rolling) {
	}

	if (playbacking) {
	}

	if (*exec_turns < previous_turns) surplus_turns++;
	previous_turns = *exec_turns;
	int key = um_wgetch(win);
	if (key == play_key) {
		if (record.count == 1) {//move to roll
			rolling = TRUE;
			timestamp--;
			goto front;
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
			front: iarc4((unsigned int )timestamp, 0);
			for (size_t question = 0; question < 51; question++) {
				rollasked[question] = FALSE;
			}
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
			current_frame = record.first;
		}

		else condensed = !condensed;
		wrefresh(win);
		return 0;
	}
	else if (key == save_key) {
		fwritep(output_paths[current_state]);
		save(current_state);
		wrefresh(win);
		return 0;
	}
	else if (key == load_key) {
		load(current_state);
		wrefresh(win);
		return 0;//redundant
	}
	else if (key == state_key) {
		iarc4((unsigned int )timestamp, 0);//TODO remove
		for (size_t question = 0; question < 51; question++) {
			answers[question] = '?';
		}
		const int attreqs[9] = {1, 7, 2, 4, 3, 0, 8, 5, 6};//Le > Ma > Wi > To > Dx > St > Pe > Ch > Ap
		for (int q = 0; q < 51; q++) {
			char result = qathing(q, attreqs);
			fprintfl(error_stream, "%d -> %c", q, result == 0 ? '!' : result);
		}

		INC(current_state, 1, states);
		wrefresh(win);
		return 0;
	}
	else if (key == unstate_key) {
		DEC(current_state, 1, states);
		wrefresh(win);
		return 0;
	}
	else if (key == menu_key) {
		/*if (inactive) {
			redrawwin(win);
			wrefresh(win);
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
		if (timestamp - record.timestamp > 0) {//prevents rewinding time
			timestamp--;
		}
		wrefresh(win);
		return 0;
	}
	else if (key == duration_key) {
		//save_quit_load();//TODO remove
		if (current_duration < 255) current_duration = (unsigned char )((current_duration + 1) * 2 - 1);
		wrefresh(win);
		return 0;
	}
	else if (key == unduration_key) {
		if (current_duration > 0) current_duration = (unsigned char )((current_duration + 1) / 2 - 1);
		wrefresh(win);
		return 0;
	}
	else if (key == quit_key) {//quits everything (stupid implementation)
		running = FALSE;
		um_endwin();
		//uninit_parent(NO_PROBLEM);
		fprintf(stdout, "Ctrl C will get you back to your beloved terminal if nothing else works.\n"); fflush(stdout);
		for (int state = 1; state < states; state++) {
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
	strcpy(previous_inputs, "");
	strcat(previous_inputs, code);
	add_key_frame(current_duration, key);
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
	uninit_gui();//TODO exit
	curs_set(1);
	return um_endwin();
}

#endif
