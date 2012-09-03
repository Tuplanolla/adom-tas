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

#include "util.h"
#include "prob.h"//problem_d
#include "log.h"//error, warning, notice
#include "cfg.h"//*
#include "asm.h"//inject_*
#include "shm.h"//shm_*, shm, state_d
#include "exec.h"
#include "rec.h"//record
#include "arc4.h"//arc4_*
#include "put.h"
#include "gui.h"
//#include "play.h"

#include "lib.h"

/**
An annotation for overloaded functions.

@param function The function to overload.
**/
#define OVERLOAD(function) function

/**
The most important variable ever defined.
**/
unsigned short int current_duration = 16;
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

/*
Prototypes for the people.
*/
int lib_uninit(void);
int lib_init(void);

/**
Uninitializes this process.

@param last Whether shared resources should also be uninitialized.
@return 0 if successful and -1 otherwise.
**/
int uninit(const bool last) {
	int result = 0;

	/*
	Unitializes the shared memory segment.
	*/
	if (shm_detach() == -1) {
		result = -1;
	}
	if (last) {
		if (shm_uninit() == -1) {
			result = -1;
		}

		/*
		Unitializes the display.
		*/
		if (curs_set(1) == ERR
				|| orig_wrefresh(stdscr) == ERR//TODO remove
				|| nocbreak() == ERR
				|| echo() == ERR
				|| orig_endwin() == ERR)
		{
			probno = log_error(UNINIT_PROBLEM);
			result = -1;
		}
	}

	/*
	Unitializes the functions.
	*/
	if (lib_uninit() == -1) {
		result = -1;
	}

	/*
	Unitializes the configuration.
	*/
	if (cfg_uninit() == -1) {
		result = -1;
	}

	return result;
}

/**
Initializes this process.

@param first Whether shared resources should also be initialized.
@return 0 if successful and -1 otherwise.
**/
int init(const bool first) {
	/*
	Initializes the configuration.
	*/
	if (cfg_init_lib() == -1) {
		return -1;
	}

	/*
	Initializes the functions.
	*/
	if (lib_init() == -1) {
		return -1;
	}

	/*
	Initializes the save-quit-load emulation.
	*/
	if (cfg_emulate_sql) {
		if (asm_inject(&save_quit_load) == -1) {
			return -1;
		}
	}
	else {
		if (asm_inject(NULL) == -1) {
			return -1;
		}
	}

	/*
	Initializes the shared memory segment.
	*/
	if (first) {
		if (shm_init() == -1) {
			return -1;
		}
	}
	if (shm_attach() == -1) {
		return -1;
	}

	/*
	Sets variables that should be automatic.
	*/
	record.timestamp = cfg_timestamp;
	options.play_on = cfg_play_instantly;
	options.play_paused = FALSE;
	options.gui_menu = FALSE;
	options.gui_info = FALSE;
	options.gui_overlay = FALSE;
	options.gui_condensed = FALSE;
	options.gui_hidden = FALSE;
	options.roll_on = FALSE;
	options.roll_cataloged = FALSE;

	return 0;
}

/**
Throws an assertion error if a function is called before it's loaded.

May be unreliable.
**/
void dlnull(void) {
	probno = log_error(ASSERT_PROBLEM);
	uninit(FALSE);
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
			do {
				bool done_quitting = TRUE;
				for (int save = 0; save < cfg_saves; save++) {
					if (shared.pids[save] != 0) {
						done_quitting = FALSE;
					}
				}
				if (done_quitting) break;
				napms(NAP_RESOLUTION / frame_rate);
			} while (TRUE);
			uninit(TRUE);
			exit(NO_PROBLEM);
		}
	}

	return 0;
}

/*
Undocumented.
*/
int copy_temporary(const int save, const bool direction) {
	for (int level = 0; level < exec_temporary_levels; level++) {
		const int offset = level * exec_temporary_parts;
		for (int part = 0; part < exec_temporary_parts; part++) {
			const int path = offset + part;
			const size_t size = strlen(cfg_exec_temporary_paths[path]) + 1
					+ intlen(save) + 1;
			char * const save_path = malloc(size);
			if (save_path == NULL) {
				probno = log_error(MALLOC_PROBLEM);
				return -1;
			}
			snprintf(save_path, size, "%s_%d",
					cfg_exec_temporary_paths[path], save);
			if (direction) {
				copy(save_path, cfg_exec_temporary_paths[path]);
			}
			else {
				copy(cfg_exec_temporary_paths[path], save_path);
			}
			free(save_path);
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
					if (shared.pids[0] == pid) {
						break;//someone activated this slot
					}
					if (shared.pids[save] != pid) {
						uninit(FALSE);//someone took this slot
						exit(NO_PROBLEM);
					}
					if (*shared.state == HAD_ENOUGH) {
						options.progress = EXIT;
						shared.pids[save] = 0;//(pid_t )
						uninit(FALSE);//everyone is shutting down
						exit(NO_PROBLEM);
					}
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
		exit(NO_PROBLEM);
	}

	return 0;
}

int play_key(WINDOW * const win) {
	if (record.current == NULL) {
		wtimeout(win, 0);
		return KEY_EOF;
	}
	int key;
	if (record.current->duration == 0) {//seed frame
		cfg_timestamp += record.current->value;
		arc4_inject((unsigned long int )cfg_timestamp, exec_arc4_calls_automatic_load);
		(*exec_saves)++;
		key = KEY_NULL;
	}
	else {//key frame
		const int delay = record.current->duration * NAP_RESOLUTION / frame_rate;
		wtimeout(win, delay);
		const int some_key = orig_wgetch(win);
		if (some_key == cfg_play_key) {
			wtimeout(win, 0);
			int some_other_key;
			do {
				some_other_key = orig_wgetch(win);
			} while (some_other_key != cfg_play_key);
		}
		else if (some_key == cfg_stop_key) {
			record.current = NULL;
		}
		key = record.current->value;
	}
	record.current = record.current->next;
	return key;
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
		if (init(TRUE) == -1) {
			uninit(FALSE);
			exit(probno);
		}
	}

	va_list	ap;
	va_start(ap, format);

	char * const buf = astresc(format);
	if (buf == NULL) {
		probno = log_error(MALLOC_PROBLEM);
		uninit(FALSE);
		exit(probno);
	}
	log_call("printf(\"%s\", ...).", buf);
	free(buf);

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
	char * const buf = astresc(path);
	if (buf == NULL) {
		probno = log_error(MALLOC_PROBLEM);
		uninit(FALSE);
		exit(probno);
	}
	log_call("unlink(\"%s\").", buf);
	free(buf);

	if (strstr(path, "ADOM.DBG") != NULL) {
		struct stat unlink_stat;
		if (stat(path, &unlink_stat) == 0) {
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
	if (options.roll_on) {
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
	orig_wrefresh(win);
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
	if (options.progress == PRINTF) {
		options.progress = WADDNSTR;
		if (gui_init() == -1) {
			uninit(FALSE);
			exit(probno);
		}
		if (init_fork() == -1) {
			uninit(FALSE);
			exit(probno);
		}
	}

	char * const buf = astresc(str);
	if (buf == NULL) {
		probno = log_error(MALLOC_PROBLEM);
		uninit(FALSE);
		exit(probno);
	}
	log_call("waddnstr(" PTRF ", \"%s\", %d).", PTRS(win), buf, n);
	free(buf);

	return orig_waddnstr(win, str, n);
}

/**
Reads a key code from a window.

@param win The window to read from.
@return The key code.
**/
int OVERLOAD(wgetch)(WINDOW * const win) {
	log_call("wgetch(" PTRF ").", PTRS(win));

	if (options.play_on) {
		int key = play_key(win);
		if (key == KEY_EOF) {
			options.play_on = FALSE;
		}
		else {
			return key;
		}
	}
	if (options.roll_on) {
		int key = play_key(win);
		if (key == KEY_EOF) {
			options.roll_on = FALSE;
		}
		else {
			return key;
		}
	}

	/*
	Keeps track of the actual turn count.
	*/
	if (*exec_turns < previous_turns) {
		negative_turns++;
	}
	else if (*exec_turns > previous_turns) {
		options.k_on = TRUE;
	}
	else {
		options.k_on = FALSE;
	}
	previous_turns = *exec_turns;
	turns = *exec_turns + negative_turns;

	/*
	Waits for a key.
	*/
	const int key = orig_wgetch(win);

	/*
	Handles a key.
	*/
	if (key == cfg_save_key) {
		put_fwrite(cfg_output_paths[current_save]);
		save_state(current_save);
	}
	else if (key == cfg_load_key) {
		load_state(current_save);
	}
	else if (key == cfg_next_save_key) {
		INC(current_save, 1, cfg_saves);
	}
	else if (key == cfg_prev_save_key) {
		DEC(current_save, 1, cfg_saves);
	}
	else if (key == cfg_longer_duration_key) {
		if (current_duration < frame_rate * frame_rate) {
			current_duration *= 2;
		}
	}
	else if (key == cfg_shorter_duration_key) {
		if (current_duration > frame_rate / frame_rate) {
			current_duration /= 2;
		}
	}
	else if (key == cfg_more_time_key) {
		if (cfg_timestamp - record.timestamp < LONG_MAX) {
			cfg_timestamp++;
		}
	}
	else if (key == cfg_less_time_key) {
		if (cfg_timestamp - record.timestamp > 0) {
			cfg_timestamp--;
		}
	}
	else if (key == cfg_menu_key) {
		options.gui_menu = !options.gui_menu;
		options.gui_info = FALSE;
	}
	else if (key == cfg_info_key) {
		options.gui_menu = FALSE;
		options.gui_info = !options.gui_info;
	}
	else if (key == cfg_condense_key) {
		options.gui_condensed = !options.gui_condensed;
	}
	else if (key == cfg_hide_key) {
		options.gui_hidden = !options.gui_hidden;
	}
	else if (key == cfg_play_key) {
		if (options.play_on) {
			options.play_on = !options.play_on;
		}
		else if (record.frames == 0) {
			options.play_on = TRUE;
			put_fread(cfg_input_path);
			record.current = record.first;
		}
	}
	else if (key == cfg_stop_key) {
		options.play_on = FALSE;
		record.current = NULL;
	}
	else if (key == cfg_quit_key) {
		options.progress = EXIT;
		*shared.state = HAD_ENOUGH;
		shared.pids[0] = 0;
		uninit(FALSE);
		exit(NO_PROBLEM);
	}
	else {
		if (record.frames == 0 && key != ' ') {
			return KEY_NULL;
		}
		else {
			const size_t inputs = sizeof previous_inputs / sizeof *previous_inputs - 1;
			for (size_t input = 0; input < inputs; input++) {//shifts the array left
				previous_inputs[input] = previous_inputs[input + 1];
			}
			previous_inputs[inputs] = key;

			rec_add_key_frame(current_duration, key);
		}
	}
	wrefresh(win);

	return key;
}

/**
Ends drawing to the screen.

Intercepts exiting prematurely.

@return OK if successful and ERR otherwise.
**/
int OVERLOAD(endwin)(void) {
	log_call("endwin().");

	curs_set(0);
	cbreak();
	noecho();
	wclear(stdscr);
	short int y, x;
	getmaxyx(stdscr, y, x);
	const char * howto = "This process has reached its end.";
	const char * more_howto = "Load another process or press the exit key to continue.";
	mvwaddstr(stdscr, y / 2 - 1, (x - strlen(howto)) / 2, howto);
	mvwaddstr(stdscr, y / 2, (x - strlen(more_howto)) / 2, more_howto);
	wrefresh(stdscr);
	while (options.progress != EXIT) {
		wgetch(stdscr);
	}
	uninit(FALSE);
	exit(NO_PROBLEM);
	return OK;
}
