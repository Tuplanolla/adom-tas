/**
Does something important.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef FCN_C
#define FCN_C

#include <stdarg.h>//va_*
#include <stdlib.h>//*env, NULL
#include <stdio.h>//*open, *close, *read, *write, FILE
#include <string.h>//str*, mem*
#include <unistd.h>//sleep, temporary kill
#include <time.h>//time_t
#include <signal.h>//temporary SIGKILL
#include <dlfcn.h>//dl*, RTLD_*
#include <sys/ioctl.h>//TIOC*
#include <sys/stat.h>//stat

#include <sys/wait.h>

#include <curses.h>//*w*, chtype, WINDOW, COLOR

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

#include "fcn.h"

void dlnull(void) {
	exit(error(ASSERT_PROBLEM));//TODO fix exit
}

intern printf_f um_printf = (void * )dlnull;
intern unlink_f um_unlink = (void * )dlnull;
intern ioctl_f um_ioctl = (void * )dlnull;
intern time_f um_time = (void * )dlnull;
intern localtime_f um_localtime = (void * )dlnull;
intern srandom_f um_srandom = (void * )dlnull;
intern random_f um_random = (void * )dlnull;
intern wrefresh_f um_wrefresh = (void * )dlnull;
intern init_pair_f um_init_pair = (void * )dlnull;
intern waddnstr_f um_waddnstr = (void * )dlnull;
intern wgetch_f um_wgetch = (void * )dlnull;
intern endwin_f um_endwin = (void * )dlnull;

/**
The active save state.
**/
int current_state = 1;

/**
The most important variables ever defined.
**/
bool was_meta = FALSE;//not good
int was_colon = FALSE;//worse
bool playbacking = FALSE;
bool rolling = FALSE;
frame_t * playback_frame;
bool running = TRUE;
char previous_inputs[77];
unsigned char dur = 15;
int surplus_turns = 0;
int previous_turns = 0;

/**
Whether an overloaded function call is the first one.
**/
bool first = TRUE, second = FALSE;

void * libc_handle;
void * libncurses_handle;

/*
Closes the dynamically linked libraries.
*/
problem_t uninit_fcn(void) {
	if (dlclose(libc_handle) != 0) {
		return error(LIBC_DLCLOSE_PROBLEM);
	}
	if (dlclose(libncurses_handle) != 0) {
		return error(LIBNCURSES_DLCLOSE_PROBLEM);
	}

	return NO_PROBLEM;
}

/*
Opens the dynamically linked libraries.
*/
problem_t init_fcn(void) {
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

	return NO_PROBLEM;
}

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
	iarc4((unsigned int )timestamp, executable_arc4_calls_automatic_load);
	(*executable_saves)++;
	add_seed_frame(timestamp);
	wrefresh(stdscr);
}

/**
Prints a formatted string.

Intercepts printing anything and initializes this process.

@param format The string format.
@return The amount of characters printed.
**/
int printf(const char * const format, ...) {
	if (first) {//TODO simplify
		first = FALSE;
		second = TRUE;
		const problem_t problem = init_parent();
		if (problem != NO_PROBLEM) {
			uninit_parent(problem);
		}
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
	if (second) {//TODO simplify
		second = FALSE;
		const problem_t problem = init_interface();
		if (problem != NO_PROBLEM) {
			uninit_parent(problem);
		}
	}

	return um_waddnstr(win, str, n);
}

int previous_key = 0;

int rollstage = 0;
bool rollasked[51];
char answers[51];
int qnum = 0;

char qathing(const int question, const int * const attreqs) {//TODO refactor without breaking
	if (rollasked[question]) return '?';
	if (answers[question] != '?') return answers[question];
	rollasked[question] = TRUE;
	int score[4] = {0, 0, 0, 0};
	for (size_t opt = 0; opt < 4; opt++) {
		int weight = 1;
		for (size_t atr = 0; atr < 9; atr++) {
			int zorg = attreqs[8 - atr];
			score[opt] += weight * executable_question_effects[question][opt][zorg];
			weight *= 2;
		}
	}
	int answer = 4;
	int max = -1 << 31;
	for (size_t opt = 0; opt < 4; opt++) {
		if (score[opt] > max) {
			max = score[opt];
			answer = (int )opt;
		}
	}
	const char letters[5] = {'a', 'b', 'c', 'd', '?'};
	answers[question] = letters[answer];
	return letters[answer];
}

/**
Reads a key code from a window.

@param win The window to read from.
@return The key code.
**/
int wgetch(WINDOW * const win) {//TODO remove bloat and refactor with extreme force
	call("wgetch(0x%08x).", (unsigned int )win);

	#define ROLL_FOR_PLAYING 0
	if (rolling) {
		rollstage++;
		switch (rollstage) {
			case -127-ROLL_FOR_PLAYING: {
				int * birthday = (int * )0x082b61f0;
				int * gender = (int * )0x082add18;
				int * race = (int * )0x082add10;
				int * prof = (int * )0x082add14;
				int * gift = (int * )0x082b6144;
				int * attributes = (int * )0x082b1728;
				int * items = (int * )0x082a5980;
				int * books = (int * )0x082a7e00;
				if (books[0x14] == 0
						|| books[0x1e] == 0
						|| items[0xa9] == 0
						|| attributes[0x01] < 20
						|| attributes[0x07] < 20) exit(0);
				char buf[32];
				snprintf(buf, sizeof buf, "cat/%u.tac", (unsigned int )timestamp);
				FILE * const f = fopen(buf, "wb");
				if (f != NULL) {
					const unsigned char header[4] = {'T', 'A', 'C', '\0'};
					fwrite(header, sizeof header, 0x01, f);
					fwrite("adom", 4, 0x01, f);
					fwrite((char [1024] ){[0 ... 1023] = '\0'}, 1, 1016, f);
					fwrite(birthday, sizeof (int), 0x01, f);
					fwrite(gender, sizeof (int), 0x01, f);
					fwrite(race, sizeof (int), 0x01, f);
					fwrite(prof, sizeof (int), 0x01, f);
					fwrite(answers, sizeof answers + 1, 0x01, f);
					fwrite(gift, sizeof (int), 0x01, f);
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
			case 8: {
				for (size_t question = 0; question < 51; question++) {
					answers[question] = '?';
				}
				return 'q';
			}
			default: {
				const int attreqs[9] = {1, 7, 2, 4, 3, 0, 8, 5, 6};//Le > Ma > Wi > To > Dx > St > Pe > Ch > Ap
				char result = qathing(executable_questions[qnum], attreqs);
				qnum++;
				if (result == '?') {
					rollstage = -128;
				}
				return result;
			}
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
		iarc4((unsigned int )timestamp, 0);
		for (size_t question = 0; question < 51; question++) {
			answers[question] = '?';
		}
		const int attreqs[9] = {1, 7, 2, 4, 3, 0, 8, 5, 6};//Le > Ma > Wi > To > Dx > St > Pe > Ch > Ap
		for (int q = 0; q < 51; q++) {
			char result = qathing(q, attreqs);
			fprintfl(error_stream, "%d -> %c", q, result == 0 ? '!' : result);
		}
		MODINC(current_state, states);
		wrefresh(win);
		return 0;
	}
	else if (key == unstate_key) {//TODO move and refactor
		WINDOW * cheat_win = newwin(rows - 5, cols, 2, 0);
		for (int row = 0; row < rows - 5; row++) {
			for (int col = 0; col < cols; col++) {
				const chtype ch = mvwinch(win, row + 2, col);
				const chtype sch = A_CHARTEXT & ch;
				if (sch == '\0' || sch == ' ') {
					const attr_t attr = COLOR_PAIR(8) | A_BOLD;
					wattron(cheat_win, attr);
					const unsigned char terrain = (*executable_terrain)[row * cols + col];
					mvwaddch(cheat_win, row, col, executable_terrain_chars[terrain]);
					const unsigned char object = (*executable_objects)[row * cols + col];
					if (object != '\0') {
						mvwaddch(cheat_win, row, col, executable_object_chars[object]);
					}
					wattroff(cheat_win, attr);
				}
				else {
					mvwaddch(cheat_win, row, col, ch);
				}
			}
		}
		for (int row = 0; row < rows - 5; row++) {
			for (int col = 0; col < cols; col++) {
				const executable_map_item_t * item = (*executable_items)[row * cols + col];
				if (item != NULL) {
					while (item->next != NULL) {
						item = item->next;
						if (item->item != NULL) {
							const executable_item_data_t i = executable_item_data[item->item->type];
							int color = i.color;
							if (color == -1) {
								color = executable_material_colors[i.material];
							}
							attr_t attr = COLOR_PAIR(color);
							if (color >= 8) {
								attr |= A_BOLD;
							}
							wattron(cheat_win, attr);
							mvwaddch(cheat_win, row, col, (chtype )executable_item_chars[i.category]);//TODO make rocks special
							wattroff(cheat_win, attr);
						}
					}
				}
			}
		}
		const executable_map_monster_t * monster = *executable_monsters;
		while (monster->next != NULL) {
			monster = monster->next;
			if (monster->monster != NULL) {
				const executable_monster_data_t m = executable_monster_data[monster->monster->type];
				attr_t attr = COLOR_PAIR(m.color);
				if (m.color >= 8) {
					attr |= A_BOLD;
				}
				wattron(cheat_win, attr);
				mvwaddch(cheat_win, monster->monster->y, monster->monster->x, (chtype )m.character);
				wattroff(cheat_win, attr);
			}
		}
		wrefresh(win);
		wrefresh(cheat_win);
		delwin(cheat_win);
		MODDEC(current_state, states);
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
		timestamp--;
		wrefresh(win);
		return 0;
	}
	else if (key == duration_key) {
		//save_quit_load();//TODO remove
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
	uninit_interface();//TODO exit
	curs_set(1);
	return um_endwin();
}

#endif
