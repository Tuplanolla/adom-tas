/**
Does something unnecessary.

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

#include "prob.h"//problem_d, PROPAGATE*
#include "log.h"//error, warning, notice
#include "cfg.h"//*
#include "asm.h"//inject_*
#include "type.h"//mode_d
#include "shm.h"//*_shm, shm
#include "rec.h"//record

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
The most important variable ever defined.
**/
unsigned short int current_duration = 15;
long int negative_turns = 0;
long int previous_turns = 0;
/**
The amount of frames at the previous input.
**/
intern unsigned int previous_frames = 0;

/**
The active save state.
**/
int current_save = 1;
/**
The four last inputs.
**/
intern int previous_inputs[4] = {0, 0, 0, 0};

imode_d imode;

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
	exit(log_error(ASSERT_PROBLEM));
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
Emulates the process of saving, quitting and loading.
**/
void save_quit_load(void) {
	/*
	The only reliable case (automatic loading) is assumed.

	Manual loading changes the random number generator's state depending on
	 the amount of available save games and
	 the actions taken in the menu.
	*/
	iarc4((unsigned long int )cfg_timestamp, exec_arc4_calls_automatic_load);
	(*exec_saves)++;
	add_seed_frame(cfg_timestamp);
	wrefresh(stdscr);
}

/**
Closes the dynamically linked libraries.

@return 0 if successful and -1 otherwise.
**/
int uninit_lib(void) {
	if (dlclose(libc_handle) != 0) {
		probno = log_error(LIBC_DLCLOSE_PROBLEM);
	}
	if (dlclose(libncurses_handle) != 0) {
		probno = log_error(LIBNCURSES_DLCLOSE_PROBLEM);
	}

	return 0;
}

/**
Opens the dynamically linked libraries.

@return 0 if successful and -1 otherwise.
**/
int init_lib(void) {
	/*
	<code>RTLD_LAZY</code> is faster than <code>RTLD_NOW</code>.
	*/
	const int mode = RTLD_LAZY;

	libc_handle = dlopen(cfg_libc_path, mode);
	if (libc_handle == NULL) {
		probno = log_error(LIBC_DLOPEN_PROBLEM);
		return -1;
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
		probno = log_error(LIBC_DLSYM_PROBLEM);
		return -1;
	}

	libncurses_handle = dlopen(cfg_libncurses_path, mode);
	if (libncurses_handle == NULL) {
		probno = log_error(LIBNCURSES_DLOPEN_PROBLEM);
		return -1;
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
		probno = log_error(LIBNCURSES_DLSYM_PROBLEM);
		return -1;
	}

	/*
	Prevents reloading libraries for child processes.
	*/
	if (unsetenv("LD_PRELOAD") == -1) {
		probno = log_warning(LD_PRELOAD_UNSETENV_PROBLEM);
	}

	return 0;
}

void uninit_curses(void) {
	const int x = curs_set(1);
	const int y = nocbreak();
	const int z = echo();
	const int c = um_endwin();
	probno = log_fprintf(cfg_error_stream, "Exiting %d, %d, %d and %d.", x, y, z, c);
}

void partial_uninit(const problem_d problem) {
	detach_shm();

	//uninit_config();

	exit(problem);
}

void uninit(const problem_d problem) {
	detach_shm();
	uninit_shm();

	//uninit_config();

	uninit_curses();
	exit(problem);
}

problem_d copy_temporary(const int state, const bool save) {
	for (unsigned int level = 0; level < exec_temporary_levels; level++) {
		const unsigned int offset = level * exec_temporary_parts;
		for (unsigned int part = 0; part < exec_temporary_parts; part++) {
			const unsigned int path = offset + part;
			const size_t size = strlen(cfg_exec_temporary_paths[path]) + 1
					+ uintlen(state) + 1;
			char * const state_path = malloc(size);
			if (state_path == NULL) {
				probno = log_error(MALLOC_PROBLEM);
				return -1;
			}
			else {
				snprintf(state_path, size, "%s_%u",
						cfg_exec_temporary_paths[path],
						state);
				if (save) {
					copy(state_path, cfg_exec_temporary_paths[path]);
				}
				else {
					copy(cfg_exec_temporary_paths[path], state_path);
				}
				free(state_path);
			}
		}
	}

	return NO_PROBLEM;
}

/**
Initializes this module.
**/
problem_d init(void) {
	/*
	Initializes the configuration.
	*/
	PROPAGATE(init_lib_config());
	record.timestamp = cfg_timestamp;

	/*
	Initializes the functions.
	*/
	PROPAGATE(init_lib());

	/*
	Enables or disables the save-quit-load emulation.
	*/
	if (cfg_sql) {
		inject_save(&save_quit_load);
	}
	else {
		//inject_save(NULL);
	}

	/*
	Initializes the shared memory segment.
	*/
	PROPAGATE(init_shm());
	PROPAGATE(attach_shm());

	return NO_PROBLEM;
}

problem_d init_fork(void) {
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
			PROPAGATE(attach_shm());

			/*
			Synchronizes with the parent process.
			*/
			while (shm.pids[0] != pid);

			return NO_PROBLEM;
		}
		else {//parent
			*shm.ppid = pid;
			shm.pids[0] = child_pid;

			/*
			Prevents defunct processes from appearing.
			*/
			signal(SIGCHLD, SIG_IGN);

			while (*shm.mode != HAD_ENOUGH) {
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

	return NO_PROBLEM;
}

/**
Saves the game to memory.
**/
int save(const int state) {
	do {
		const pid_t child_pid = fork();
		if (child_pid == -1) {
			probno = log_error(FORK_PROBLEM);
			return -1;
		}
		else {
			pid = getpid();
			if (child_pid == 0) {//child
				PROPAGATE(attach_shm());

				while (shm.pids[0] != pid);

				return NO_PROBLEM;
			}
			else {//parent
				shm.pids[0] = child_pid;
				shm.pids[state] = pid;

				signal(SIGCHLD, SIG_IGN);

				/*
				Saves the temporary files.
				*/
				PROPAGATE(copy_temporary(state, TRUE));

				while (TRUE) {
					if (shm.pids[0] == pid) break;//someone activated this slot
					if (shm.pids[state] != pid) partial_uninit(NO_PROBLEM);//someone took this slot
					if (*shm.mode == HAD_ENOUGH) partial_uninit(NO_PROBLEM);//everyone is shutting down
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
						shm.chs[state][row][col] = mvwinch(stdscr, row, col);
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
	} while (cfg_preserve);

	return 0;
}

/**
Loads the game from memory.
**/
int load(const int state) {
	if (shm.pids[state] != 0) {
		/*
		Loads the temporary files.
		*/
		PROPAGATE(copy_temporary(state, FALSE));

		const pid_t parent_pid = shm.pids[state];
		shm.pids[state] = (pid_t )0;
		shm.pids[0] = parent_pid;

		//TODO empty screen

		partial_uninit(NO_PROBLEM);
	}

	return 0;
}

/**
Prints a formatted string.

Intercepts printing anything and initializes this process.

@param format The string format.
@return The amount of characters printed.
**/
int printf(const char * const format, ...) {
	if (!initializing && !initialized) {
		initializing = TRUE;
		initialized = FALSE;
		PROPAGATEC(init(), uninit);
	}

	va_list	ap;
	va_start(ap, format);

	log_call("printf(...).");

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
int unlink(const char * const path) {
	log_call("unlink(\"%s\").", path);

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
@return 0 if successful and -1 otherwise.
**/
int ioctl(const int d, const unsigned long request, ...) {
	va_list	argp;
	va_start(argp, request);
	void * arg = va_arg(argp, void *);

	log_call("ioctl(" PTRF ", " PTRF ", " PTRF ").",
			PTRS(d), PTRS(request), PTRS(arg));

	const int result = um_ioctl(d, request, arg);
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
time_t time(time_t * const t) {
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
struct tm * localtime(const time_t * const timep) {
	log_call("localtime(" PTRF ").", PTRS(timep));

	return gmtime(timep);//reduces entropy
}

/**
Seeds the pseudorandom number generator.

@param seed The seed.
**/
void srandom(const unsigned int seed) {
	log_call("srandom(%u).", seed);

	um_srandom(seed);
}

/**
Generates the next pseudorandom number.

@return The number.
**/
long int random(void) {
	log_call("random().");

	return um_random();
}

bool skipwr = FALSE;

/**
Redraws the window.

Draws the custom interface.

@param win The window to redraw.
@return 0 if successful and -1 otherwise.
**/
int wrefresh(WINDOW * const win) {
	if (skipwr) return 0;
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
	const int result = um_wrefresh(win);
	draw_gui(win);
	wmove(win, y, x);
	wattr_set(win, attrs, pair, NULL);

	return result;
}

/**
Initializes a new color pair.

@param pair The index of the pair.
@param f The foreground color.
@param b The background color.
@return 0 if successful and -1 otherwise.
**/
int init_pair(const short int pair, const short int f, const short int b) {
	log_call("init_pair(%d, %d, %d).", pair, f, b);

	pairs++;
	return um_init_pair(pair, f, b);
}

/**
Prints a string to a window.

Draws the custom interface.

@param win The window to print to.
@param str The string to print.
@param n The length of the string.
@return 0 if successful and -1 otherwise.
**/
int waddnstr(WINDOW * const win, const char * const str, const int n) {
	log_call("waddnstr(" PTRF ", %s, %d).", PTRS(win), str, n);

	if (initializing && !initialized) {
		initializing = FALSE;
		initialized = TRUE;
		PROPAGATEC(init_fork(), uninit);
		PROPAGATEC(init_gui(), uninit);
	}

	return um_waddnstr(win, str, n);
}

/**
Reads a key code from a window.

@param win The window to read from.
@return The key code.
**/
int wgetch(WINDOW * const win) {
	log_call("wgetch(" PTRF ").", PTRS(win));

	if (playing) {
		playing = next_key(win) != KEY_EOF;
	}
	if (rolling) {
		//roll
	}

	/**
	Keeps track of the actual turn count.
	**/
	if (*exec_turns < previous_turns) {
		negative_turns++;
	}
	else if (*exec_turns > previous_turns) {
		in_game = TRUE;
	}
	else {
		in_game = FALSE;
	}
	previous_turns = *exec_turns;
	turns = *exec_turns + negative_turns;

	const int key = um_wgetch(win);
	if (key == cfg_play_key) {
		if (record.frames == 0) {
			playing = TRUE;
			put_fread(cfg_input_path);
			record.current = record.first;
		}

		else condensed = !condensed;
	}
	else if (key == cfg_save_key) {
		put_fwrite(cfg_output_paths[current_save]);
		save(current_save);
	}
	else if (key == cfg_load_key) {
		load(current_save);//redundant
	}
	else if (key == cfg_state_key) {
		INC(current_save, 1, cfg_saves);
	}
	else if (key == cfg_unstate_key) {
		DEC(current_save, 1, cfg_saves);
	}
	else if (key == cfg_duration_key) {
		if (current_duration < UCHAR_MAX) {
			current_duration = (unsigned char )((current_duration + 1) * 2 - 1);
		}
	}
	else if (key == cfg_unduration_key) {
		if (current_duration > 0) {
			current_duration = (unsigned char )((current_duration + 1) / 2 - 1);
		}
	}
	else if (key == cfg_time_key) {
		if (cfg_timestamp - record.timestamp < INT_MAX) {//prevents rewinding time
			cfg_timestamp++;
		}
	}
	else if (key == cfg_untime_key) {
		if (cfg_timestamp - record.timestamp > 0) {//prevents rewinding time
			cfg_timestamp--;
		}
	}
	else if (key == cfg_menu_key) {
		inactive = !inactive;
	}
	else if (key == cfg_condense_key) {
		condensed = !condensed;
	}
	else if (key == cfg_hide_key) {
		hidden = !hidden;
	}
	else if (key == cfg_play_key) {
		paused = !paused;
	}
	else if (key == cfg_stop_key) {
		playing = FALSE;
		record.current = NULL;
	}
	else if (key == cfg_quit_key) {
		quitting = TRUE;
		*shm.mode = HAD_ENOUGH;
		shm.pids[current_save] = 0;
		partial_uninit(NO_PROBLEM);
	}
	else if (!inactive) {
		if (record.frames > 0 || key == ' ') {
			const size_t inputs = sizeof previous_inputs / sizeof *previous_inputs - 1;
			for (size_t input = 0; input < inputs; input++) {//shifts the array left
				previous_inputs[input] = previous_inputs[input + 1];
			}
			previous_inputs[inputs] = key;

			add_key_frame(current_duration, key);
		}

		return key;
	}
	wrefresh(win);
	return 0;
}

/**
Ends drawing to the screen.

Intercepts exiting prematurely.

@return <code>OK</code> if successful and <code>ERR</code> otherwise.
**/
int endwin(void) {
	log_call("endwin().");

	noecho();
	wclear(stdscr);
	while (!quitting) {
		wgetch(stdscr);
	}
	partial_uninit(NO_PROBLEM);
	return OK;
}
