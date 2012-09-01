/**
Does something unnecessary.

@file lib.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#include <stdarg.h>//va_*, exit
#include <limits.h>//*_MAX
#include <stdlib.h>//*env, NULL
#include <stdio.h>//*open, *close, *read, *write, FILE
#include <string.h>//str*, mem*
#include <unistd.h>//fork, sleep
#include <time.h>//time_t
#include <signal.h>//sig*, SIG*
#include <dlfcn.h>//dl*, RTLD_*
#include <sys/ioctl.h>//TIOC*
#include <sys/stat.h>//stat

#include <sys/wait.h>

#include <curses.h>//*w*, chtype, WINDOW, COLOR

#include "prob.h"//problem_d
#include "log.h"//error, warning, notice
#include "cfg.h"//*
#include "asm.h"//inject_*
#include "shm.h"//shm_*, shm, state_d
#include "rec.h"//record
#include "arc4.h"//arc4_*

#include "util.h"
#include "exec.h"
#include "shm.h"
#include "prob.h"
#include "rec.h"
#include "put.h"
#include "log.h"
#include "cfg.h"
#include "play.h"
#include "gui.h"

#define UM_ALIAS
#include "lib.h"
#undef UM_ALIAS

/**
An annotation for overloaded functions.

@param function The function to overload.
**/
#define OVERLOAD(function) function

/**
The most important variable ever defined.
**/
unsigned short int current_duration = 15;
long int negative_turns = 0;
long int previous_turns = 0;
/**
The amount of frames at the previous input.
**/
unsigned int previous_record_frames = 0;

/**
The active save state.
**/
int current_save = 1;
/**
The four last inputs.
**/
int previous_inputs[4] = {0, 0, 0, 0};

options_d options = {
	.progress = MAIN
};

/**
Some dlfcn stuff.
**/
void * libc_handle;
void * libncurses_handle;

pid_t pid = 0;

/**
Throws an assertion error if a function is called before it's loaded.

May be unreliable.
**/
void dlnull(void) {
	probno = log_error(ASSERT_PROBLEM);
	//uninit(TRUE);
	exit(probno);
}

printf_f orig_printf = (printf_f )dlnull;
unlink_f orig_unlink = (unlink_f )dlnull;
ioctl_f orig_ioctl = (ioctl_f )dlnull;
time_f orig_time = (time_f )dlnull;
localtime_f orig_localtime = (localtime_f )dlnull;
srandom_f orig_srandom = (srandom_f )dlnull;
random_f orig_random = (random_f )dlnull;
wrefresh_f orig_wrefresh = (wrefresh_f )dlnull;
init_pair_f orig_init_pair = (init_pair_f )dlnull;
waddnstr_f orig_waddnstr = (waddnstr_f )dlnull;
wgetch_f orig_wgetch = (wgetch_f )dlnull;
endwin_f orig_endwin = (endwin_f )dlnull;

/**
Emulates the process of saving, quitting and loading.
**/
void save_quit_load(void) {
	/*
	The only reliable case (automatic loading) is assumed.

	Manual loading changes the random number generator's state depending on
	 the amount of available save games and
	 the actions taken in the menu.
	*/
	arc4_inject((unsigned long int )cfg_timestamp, exec_arc4_calls_automatic_load);
	(*exec_saves)++;
	rec_add_seed_frame(cfg_timestamp);
	wrefresh(stdscr);
}

/**
Uninitializes the original functions.

@return 0 if successful and -1 otherwise.
**/
int lib_uninit(void) {
	int result = 0;

	/*
	Closes the dynamically linked libraries.
	*/
	if (dlclose(libc_handle) != 0) {
		probno = log_error(LIBC_DLCLOSE_PROBLEM);
		result = -1;
	}

	if (dlclose(libncurses_handle) != 0) {
		probno = log_error(LIBNCURSES_DLCLOSE_PROBLEM);
		result = -1;
	}

	return result;
}

/**
Initializes the original functions.

@return 0 if successful and -1 otherwise.
**/
int lib_init(void) {
	/*
	<code>RTLD_LAZY</code> is faster than <code>RTLD_NOW</code>.
	*/
	const int mode = RTLD_LAZY;

	/*
	Opens the dynamically linked libraries.
	*/
	libc_handle = dlopen(cfg_libc_path, mode);
	if (libc_handle == NULL) {
		probno = log_error(LIBC_DLOPEN_PROBLEM);
		return -1;
	}
	orig_printf = (printf_f )dlsym(libc_handle, "printf");
	orig_unlink = (unlink_f )dlsym(libc_handle, "unlink");
	orig_ioctl = (ioctl_f )dlsym(libc_handle, "ioctl");
	orig_time = (time_f )dlsym(libc_handle, "time");
	orig_localtime = (localtime_f )dlsym(libc_handle, "localtime");
	orig_srandom = (srandom_f )dlsym(libc_handle, "srandom");
	orig_random = (random_f )dlsym(libc_handle, "random");
	if (orig_printf == NULL
			|| orig_unlink == NULL
			|| orig_time == NULL
			|| orig_localtime == NULL
			|| orig_srandom == NULL
			|| orig_random == NULL
			|| orig_ioctl == NULL) {
		probno = log_error(LIBC_DLSYM_PROBLEM);
		return -1;
	}

	libncurses_handle = dlopen(cfg_libncurses_path, mode);
	if (libncurses_handle == NULL) {
		probno = log_error(LIBNCURSES_DLOPEN_PROBLEM);
		return -1;
	}
	orig_wrefresh = (wrefresh_f )dlsym(libncurses_handle, "wrefresh");
	orig_init_pair = (init_pair_f )dlsym(libncurses_handle, "init_pair");
	orig_waddnstr = (waddnstr_f )dlsym(libncurses_handle, "waddnstr");
	orig_wgetch = (wgetch_f )dlsym(libncurses_handle, "wgetch");
	orig_endwin = (endwin_f )dlsym(libncurses_handle, "endwin");
	if (orig_init_pair == NULL
			|| orig_wrefresh == NULL
			|| orig_wgetch == NULL
			|| orig_endwin == NULL) {
		probno = log_error(LIBNCURSES_DLSYM_PROBLEM);
		return -1;
	}

	/*
	Prevents reloading the libraries for child processes.
	*/
	if (unsetenv("LD_PRELOAD") == -1) {
		probno = log_warning(LD_PRELOAD_UNSETENV_PROBLEM);
	}

	return 0;
}

/**
Uninitializes this library.

@return 0 if successful and -1 otherwise.
**/
int uninit(const bool clean) {
	int result = 0;
	if (curs_set(1) == ERR
			|| orig_wrefresh(stdscr) == ERR//TODO remove
			|| nocbreak() == ERR
			|| echo() == ERR
			|| orig_endwin() == ERR)
	{
		probno = log_error(UNINIT_PROBLEM);
		result = -1;
	}
	if (shm_detach() == -1) {
		result = -1;
	}
	if (clean) {
		if (shm_uninit() == -1) {
			result = -1;
		}
	}
	if (lib_uninit() == -1) {
		result = -1;
	}
	if (cfg_uninit() == -1) {
		result = -1;
	}

	return result;
}

/**
Initializes this library.

@return 0 if successful and -1 otherwise.
**/
int init(void) {
	int result = 0;

	/*
	Initializes the configuration.
	*/
	cfg_init_lib();
	record.timestamp = cfg_timestamp;

	/*
	Initializes the functions.
	*/
	lib_init();

	/*
	Enables or disables the save-quit-load emulation.
	*/
	if (cfg_emulate_sql) {
		asm_inject(&save_quit_load);
	}
	else {
		//inject_save(NULL);
	}

	/*
	Initializes the shared memory segment.
	*/
	shm_init();
	shm_attach();

	return result;
}

/**
Forks things.

@return 0 if successful and -1 otherwise.
**/
int init_fork(void) {
	/*
	Creates a process monitor to avoid accidental daemonization.
	*/
	const pid_t child_pid = fork();
	if (child_pid == -1) {
		probno = log_error(FORK_PROBLEM);
		return -1;
	}
	else {
		pid = getpid();
		if (child_pid == 0) {//child
			shm_attach();

			/*
			Synchronizes with the parent process.
			*/
			while (shared.pids[0] != pid);

			return NO_PROBLEM;
		}
		else {//parent
			*shared.ppid = pid;
			shared.pids[0] = child_pid;

			/*
			Prevents defunct processes from appearing.
			*/
			signal(SIGCHLD, SIG_IGN);

			while (*shared.state != HAD_ENOUGH) {
				napms(NAP_RESOLUTION / frame_rate);
			}
			/*do {
				bool done_quitting = TRUE;
				for (int state = 0; state < states; state++) {
					if (shm.pids[state] != 0) {
						done_quitting = FALSE;
					}
				}
				if (done_quitting) break;
				napms(TIMER_RATE / frame_rate);
			} while (TRUE);*/
			uninit(NO_PROBLEM);
		}
	}

	return 0;
}

/*
Undocumented.
*/
int copy_temporary(const int save, const bool direction) {
	for (unsigned int level = 0; level < exec_temporary_levels; level++) {
		const unsigned int offset = level * exec_temporary_parts;
		for (unsigned int part = 0; part < exec_temporary_parts; part++) {
			const unsigned int path = offset + part;
			const size_t size = strlen(cfg_exec_temporary_paths[path]) + 1
					+ uintlen(save) + 1;
			char * const save_path = malloc(size);
			if (save_path == NULL) {
				probno = log_error(MALLOC_PROBLEM);
				return -1;
			}
			else {
				snprintf(save_path, size, "%s_%u",
						cfg_exec_temporary_paths[path],
						save);
				if (direction) {
					copy(save_path, cfg_exec_temporary_paths[path]);
				}
				else {
					copy(cfg_exec_temporary_paths[path], save_path);
				}
				free(save_path);
			}
		}
	}

	return 0;
}

/**
Saves the game to memory.

@param save The desired save state.
@return 0 if successful and -1 otherwise.
**/
int save_state(const int save) {
	do {
		const pid_t child_pid = fork();
		if (child_pid == -1) {
			probno = log_error(FORK_PROBLEM);
			return -1;
		}
		else {
			pid = getpid();
			if (child_pid == 0) {//child
				shm_attach();

				while (shared.pids[0] != pid);

				return NO_PROBLEM;
			}
			else {//parent
				shared.pids[0] = child_pid;
				shared.pids[save] = pid;

				signal(SIGCHLD, SIG_IGN);

				/*
				Saves the temporary files.
				*/
				copy_temporary(save, TRUE);

				while (TRUE) {
					if (shared.pids[0] == pid) break;//someone activated this slot
					if (shared.pids[save] != pid) uninit(FALSE);//someone took this slot
					if (*shared.state == HAD_ENOUGH) uninit(FALSE);//everyone is shutting down
					napms(NAP_RESOLUTION / frame_rate);
				}

				/*
				Stores the position of the cursor.
				*/
				short int y, x;
				getyx(stdscr, y, x);

				/*
				Saves the screen.
				*/
				for (int row = 0; row < cfg_rows; row++) {
					for (int col = 0; col < cfg_cols; col++) {
						shared.chs[save][row][col] = mvwinch(stdscr, row, col);
					}
				}

				/*
				Restores the position of the cursor.
				*/
				wmove(stdscr, y, x);

				/*
				Redraws the window.
				*/
				redrawwin(stdscr);
				wrefresh(stdscr);
			}
		}
	} while (cfg_keep_saves);

	return 0;
}

/**
Loads the game from memory.

@param save The desired save state.
@return 0 if successful and -1 otherwise.
**/
int load_state(const int save) {
	if (shared.pids[save] != 0) {
		/*
		Loads the temporary files.
		*/
		copy_temporary(save, FALSE);

		const pid_t parent_pid = shared.pids[save];
		shared.pids[save] = (pid_t )0;
		shared.pids[0] = parent_pid;

		//TODO empty screen

		uninit(FALSE);
	}

	return 0;
}

/**
Prints a formatted string.

Intercepts printing anything and initializes this process.

@param format The string format.
@return The amount of characters printed.
**/
int OVERLOAD(printf)(const char * const format, ...) {
	if (options.progress == MAIN) {
		options.progress = PRINTF;
		if (init() == -1) {
			uninit(0);
			exit(-1);
		}
	}

	va_list	ap;
	va_start(ap, format);

	log_call("printf(%s).", format);//TODO parse

	const int result = vsnprintf(NULL, 0, format, ap);
	va_end(ap);
	return result;
}

/**
Removes a file.

Intercepts removing the debug file if it exists.

@param path The path of the file to remove.
@return 0 if successful and -1 otherwise.
**/
int OVERLOAD(unlink)(const char * const path) {
	log_call("unlink(\"%s\").", path);

	if (strcmp(path, "ADOM.DBG") == 0) {
		struct stat buf;
		if (stat(path, &buf) == 0) {
			sleep(1);
			return 0;
		}
	}
	return orig_unlink(path);
}

/**
Controls the terminal.

Intercepts <code>TIOCGWINSZ</code> to always report a fixed size.
Resizing the terminal causes spurious calls and prints garbage on the screen.

@param fildes An open file descriptor.
@param request A request conforming to <code>ioctl_list</code>.
@param ... A single pointer.
@return 0 if successful and -1 otherwise.
**/
int OVERLOAD(ioctl)(const int fildes, const unsigned long request, ...) {
	va_list	argp;
	va_start(argp, request);
	void * arg = va_arg(argp, void *);

	log_call("ioctl(" PTRF ", " PTRF ", " PTRF ").",
			PTRS(fildes), PTRS(request), PTRS(arg));

	const int result = orig_ioctl(fildes, request, arg);
	if (request == TIOCGWINSZ) {
		struct winsize * size = (struct winsize * )arg;
		size->ws_row = (unsigned short int )cfg_rows;
		size->ws_col = (unsigned short int )cfg_cols;
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
time_t OVERLOAD(time)(time_t * const t) {
	log_call("time(" PTRF ").", PTRS(t));

	if (t != NULL) {
		*t = cfg_timestamp;
	}
	return cfg_timestamp;//reduces entropy
}

/**
Converts a <code>time_t</code> to a broken-down <code>struct tm</code>.

Replaces <code>localtime</code> with <code>gmtime</code> to disregard timezones.

@param timep The <code>time_t</code> to convert.
@return The <code>struct tm</code>.
**/
struct tm * OVERLOAD(localtime)(const time_t * const timep) {
	log_call("localtime(" PTRF ").", PTRS(timep));

	return gmtime(timep);//reduces entropy
}

/**
Seeds the pseudorandom number generator.

@param seed The seed.
**/
void OVERLOAD(srandom)(const unsigned int seed) {
	log_call("srandom(%u).", seed);

	orig_srandom(seed);
}

/**
Generates the next pseudorandom number.

@return The number.
**/
long int OVERLOAD(random)(void) {
	log_call("random().");

	return orig_random();
}

/**
Redraws the window.

Draws the custom interface.

@param win The window to redraw.
@return OK if successful and ERR otherwise.
**/
int OVERLOAD(wrefresh)(WINDOW * const win) {
	if (options.roll_active) {
		return OK;
	}

	log_call("wrefresh(" PTRF ").", PTRS(win));

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
	attr_t attrs; attr_t * const attrs_ptr = &attrs;
	short int pair; short int * const pair_ptr = &pair;
	wattr_get(win, attrs_ptr, pair_ptr, NULL);
	getyx(win, y, x);
	wattrset(win, A_NORMAL);
	const int result = orig_wrefresh(win);
	gui_draw(win);
	wmove(win, y, x);
	wattr_set(win, attrs, pair, NULL);

	return result;
}

/**
Initializes a new color pair.

@param pair The index of the pair.
@param f The foreground color.
@param b The background color.
@return OK if successful and ERR otherwise.
**/
int OVERLOAD(init_pair)(const short int pair, const short int f, const short int b) {
	log_call("init_pair(%d, %d, %d).", pair, f, b);

	pairs++;
	return orig_init_pair(pair, f, b);
}

/**
Prints a string to a window.

Draws the custom interface.

@param win The window to print to.
@param str The string to print.
@param n The length of the string.
@return OK if successful and ERR otherwise.
**/
int OVERLOAD(waddnstr)(WINDOW * const win, const char * const str, const int n) {
	log_call("waddnstr(" PTRF ", %s, %d).", PTRS(win), str, n);

	if (options.progress == PRINTF) {
		options.progress = WADDNSTR;
		if (init_fork() == -1) {
			return ERR;
		}
		if (gui_init() == -1) {
			return ERR;
		}
	}

	return orig_waddnstr(win, str, n);
}

/**
Reads a key code from a window.

@param win The window to read from.
@return The key code.
**/
int OVERLOAD(wgetch)(WINDOW * const win) {
	log_call("wgetch(" PTRF ").", PTRS(win));

	if (options.record_active) {
		options.record_active = next_key(win) != KEY_EOF;
	}
	if (options.roll_active) {
		//roll
	}

	/**
	Keeps track of the actual turn count.
	**/
	if (*exec_turns < previous_turns) {
		negative_turns++;
	}
	else if (*exec_turns > previous_turns) {
		options.key_active = TRUE;
	}
	else {
		options.key_active = FALSE;
	}
	previous_turns = *exec_turns;
	turns = *exec_turns + negative_turns;

	const int key = orig_wgetch(win);
	if (key == cfg_play_key) {
		if (record.frames == 0) {
			options.record_active = TRUE;
			put_fread(cfg_input_path);
			record.current = record.first;
		}

		else options.gui_condensed = !options.gui_condensed;
	}
	else if (key == cfg_save_key) {
		put_fwrite(cfg_output_paths[current_save]);
		save_state(current_save);
	}
	else if (key == cfg_load_key) {
		load_state(current_save);//redundant
	}
	else if (key == cfg_next_save_key) {
		INC(current_save, 1, cfg_saves);
	}
	else if (key == cfg_prev_save_key) {
		DEC(current_save, 1, cfg_saves);
	}
	else if (key == cfg_longer_duration_key) {
		if (current_duration < UCHAR_MAX) {
			current_duration *= 2;
		}
	}
	else if (key == cfg_shorter_duration_key) {
		if (current_duration > 0) {
			current_duration /= 2;
		}
	}
	else if (key == cfg_more_time_key) {
		if (cfg_timestamp - record.timestamp < INT_MAX) {//prevents rewinding time
			cfg_timestamp++;
		}
	}
	else if (key == cfg_less_time_key) {
		if (cfg_timestamp - record.timestamp > 0) {//prevents rewinding time
			cfg_timestamp--;
		}
	}
	else if (key == cfg_menu_key) {
		options.gui_active = !options.gui_active;
	}
	else if (key == cfg_condense_key) {
		options.gui_condensed = !options.gui_condensed;
	}
	else if (key == cfg_hide_key) {
		options.gui_hidden = !options.gui_hidden;
	}
	else if (key == cfg_play_key) {
		options.record_paused = !options.record_paused;
	}
	else if (key == cfg_stop_key) {
		options.record_active = FALSE;
		record.current = NULL;
	}
	else if (key == cfg_quit_key) {
		options.progress = EXIT;
		*shared.state = HAD_ENOUGH;
		shared.pids[current_save] = 0;
		uninit(FALSE);
	}
	else if (!options.gui_active) {
		if (record.frames > 0 || key == ' ') {
			const size_t inputs = sizeof previous_inputs / sizeof *previous_inputs - 1;
			for (size_t input = 0; input < inputs; input++) {//shifts the array left
				previous_inputs[input] = previous_inputs[input + 1];
			}
			previous_inputs[inputs] = key;

			rec_add_key_frame(current_duration, key);
		}

		return key;
	}
	wrefresh(win);
	return 0;
}

/**
Ends drawing to the screen.

Intercepts exiting prematurely.

@return OK if successful and ERR otherwise.
**/
int OVERLOAD(endwin)(void) {
	log_call("endwin().");

	noecho();
	wclear(stdscr);
	while (!options.progress == EXIT) {
		wgetch(stdscr);
	}
	uninit(FALSE);
	return OK;
}
