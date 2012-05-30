/**
Provides.

Put close(shm_fd); somewhere.
**/
#ifndef __C
#define __C

#include <curses.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <libgen.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "config.h"
#include "adom.h"
#include "adom-tas.h"

INIT_PAIR real_init_pair;
WCLEAR real_wclear;
WREFRESH real_wrefresh; 
WMOVE real_wmove;
WADDCH real_waddch;
WADDNSTR real_waddnstr;
WINCH real_winch;
WGETCH real_wgetch;
WGETNSTR real_wgetnstr;
SPRINTF real_sprintf;
UNLINK real_unlink;
TIME real_time;
IOCTL real_ioctl;

/**
Annotates overloaded functions.
**/
#define OVERLOAD(function) function

struct shm_s {
	int pids[SAVE_STATES];
};
typedef struct shm_s shm_t;

/**
Lists boolean types.
Apparently some other library already does this.
**/
/*
enum bool_e {
	FALSE,
	TRUE,
};
typedef enum bool_e bool;
*/

/**
Converts an error code to an error message.
@param error The error code.
@return The error message.
**/
const char *error_message(const error_t code) {
	if (code == NO_ERROR) return "Nothing failed.";
	else if (code == SETENV_ERROR) return "Setting the environment variable LD_PRELOAD failed.";
	else if (code == USER_ID_ERROR) return "Linking dynamic libraries failed due to conflicting user identifiers.";
	else if (code == DLOPEN_ERROR) return "Loading dynamically linked libraries failed.";
	else if (code == STAT_ERROR) return "Reading the executable failed.";
	else if (code == WRONG_VERSION_ERROR) return "Identifying the executable by its version failed.";
	else if (code == WRONG_SIZE_ERROR) return "Identifying the executable by its size failed.";
	else return NULL;
}

/**
Prints an error message and forwards its error code.
@param error The error code.
@return The error code.
**/
const error_t propagate(const error_t code) {
	fprintf(stderr, "Error: %s\n", error_message(code));
	return code;
}

/**
Logs a message.
@param message The message.
@return TRUE if the message was logged successfully and FALSE otherwise.
**/
char *log_file = LOG_PATH;
const bool printl(const char *message) {
	if (log_file == NULL) fprintf(stderr, "LOG: %s\n", message);//TODO add a file_exists check
	else {//TODO write to a file properly
		FILE *handle = fopen(log_file, "a");
		fwrite(message, 1, strlen(message), handle);
		fclose(handle);
	}
	return TRUE;
}

/**
Shares memory or something.
**/
int shm_fd;//some handle
shm_t *conf;//shared
void shmup() {
	bool first = FALSE;

	shm_fd = shm_open(SHM_PATH, O_RDWR|O_CREAT|O_EXCL, S_IRUSR|S_IWUSR);
	if (shm_fd > 0) {
		first = TRUE;
	}
	shm_fd = shm_open(SHM_PATH, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
	if (shm_fd < 0) {
		printl("Failed to get what is rightfully mine.\n");
		return;//error here
	}

	ftruncate(shm_fd, sizeof (shm_t));

	conf = (shm_t *)mmap(NULL, sizeof (shm_t), PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (conf == MAP_FAILED) {
		printl("Failed to remember.\n");
		return;//error here
	}

	printl("Map");
	if (first) {
		for (int index = 0; index < SAVE_STATES; index++) conf->pids[index] = NULL;
		printl("-reset-");//doesn't work
	}
	else {
		printl("-read-");
	}
	printl("ped.\n");
}

/*
Returns the key code of a key number.
@param code The key code to return. Requires five bytes of memory at most.
@param key The key number.
*/
#define STRCPY_RETURN(variable) {\
		strcpy(code, variable);\
		return code;\
	}
#define SPRINTF_RETURN(format, variable) {\
		real_sprintf(code, format, variable);\
		return code;\
	}
void *key_code(const char *code, const int key) {
	if (key == '\\') STRCPY_RETURN("\\\\");//backslash
	if (key == KEY_UP) STRCPY_RETURN("\\U");//Up
	if (key == KEY_DOWN) STRCPY_RETURN("\\D");//Down
	if (key == KEY_LEFT) STRCPY_RETURN("\\L");//Left
	if (key == KEY_RIGHT) STRCPY_RETURN("\\R");//Right
	if (key == ' ') STRCPY_RETURN("\\S");//Space
	if (key == 0x1b) STRCPY_RETURN("\\M");//Meta (Alt or Esc)
	if (key == 0x7f) STRCPY_RETURN("\\C_");//Delete
	if (key == KEY_A1) STRCPY_RETURN("\\H");//keypad Home
	if (key == KEY_A3) STRCPY_RETURN("\\+");//keypad PageUp
	if (key == KEY_B2) STRCPY_RETURN("\\.");//keypad center
	if (key == KEY_C1) STRCPY_RETURN("\\E");//keypad End
	if (key == KEY_C3) STRCPY_RETURN("\\-");//keypad PageDown
	if (key >= 0x00 && key < 0x1f) SPRINTF_RETURN("\\C%c", (char )(0x60+key));//control keys
	if (key >= KEY_F(1) && key <= KEY_F(64)) SPRINTF_RETURN("\\%d", key-KEY_F(0));//function keys
	if (key > 0x20 && key < 0x80) SPRINTF_RETURN("%c", (char )key);//printable keys
	SPRINTF_RETURN("\\x%02x", key&0xff);//nonprintable keys
}

/**
Loads functions from dynamically linked libraries (libc and libncurses).
**/
void load_dynamic_libraries() {
	FILE *_handle = fopen(log_file, "w");//I don't belong here.
	fclose(_handle);

	char *libc_path;
	char *curses_path;

	/*
	Extracts library paths from environment variables.
	*/
	libc_path = getenv("LIBC_PATH");
	if (libc_path == NULL) libc_path = LIBC_PATH;
	curses_path = getenv("CURSES_PATH");
	if (curses_path == NULL) curses_path = LIBNCURSES_PATH;

	void *handle;

	/*
	Imports functions from libc.
	*/
	handle = (void *)dlopen(libc_path, RTLD_LAZY);//requires either RTLD_LAZY or RTLD_NOW
	if (handle == NULL) propagate(DLOPEN_ERROR);
	real_sprintf = (SPRINTF )dlsym(handle, "sprintf");
	real_unlink = (UNLINK )dlsym(handle, "unlink");
	real_time = (TIME )dlsym(handle, "time");
	real_ioctl = (TIME )dlsym(handle, "ioctl");

	/*
	Imports functions from libncurses.
	*/
	handle = (void *)dlopen(curses_path, RTLD_LAZY);
	if (handle == NULL) propagate(DLOPEN_ERROR);
	real_init_pair = (INIT_PAIR )dlsym(handle, "init_pair");
	real_wclear = (WCLEAR )dlsym(handle, "wclear");
	real_wrefresh = (WREFRESH )dlsym(handle, "wrefresh");
	real_wmove = (WMOVE )dlsym(handle, "wmove");
	real_waddch = (WADDCH )dlsym(handle, "waddch");
	real_waddnstr = (WADDNSTR )dlsym(handle, "waddnstr");
	real_winch = (WINCH )dlsym(handle, "winch");
	real_wgetch = (WGETCH )dlsym(handle, "wgetch");
	real_wgetnstr = (WGETNSTR )dlsym(handle, "wgetnstr");

	/*
	Prevents reloading libraries for child processes.
	*/
	unsetenv("LD_PRELOAD");
}

/**
Guarantees a single call to load the dynamically linked libraries.
**/
bool initialized = FALSE;
void initialize() {
	if (initialized) return;
	initialized = TRUE;

	load_dynamic_libraries();
}

/*
Debug messages pop up on the screen at random locations.
It's a feature, not a bug.
*/
bool tired = TRUE;
void continuator(const int signo) {
	if (signo == SIGCONT) {
		printl("Caught this.\n");
		tired = FALSE;
	}
}

/**
Saves the game to memory.
**/
void save(const int state) {
	printf("*"); fflush(stdout);//a graceful puff of smoke
	pid_t pid = fork();
	shmup();
	if (pid != NULL) {//parent
		//printf("DEBUG: parent(%i); ", (int )getpid()); fflush(stdout);
		//printf("DEBUG: parent(%i).stop(); ", (int )getpid()); fflush(stdout);
		conf->pids[state] = getpid();
		if (signal(SIGCONT, continuator) == SIG_ERR) printl("Can't catch this.\n");
		printf("<%i fell asleep>", (int )getpid()); fflush(stdout);
		while (tired) nanosleep(1000000000/60);
		printf("<%i woke up>", (int )getpid()); fflush(stdout);
		conf->pids[0] = getpid();
	}
	else {//child
		//printf("DEBUG: child(%i); ", (int )getpid()); fflush(stdout);
		printf("<%i is ready>", (int )getpid()); fflush(stdout);
		conf->pids[0] = getpid();
	}
}

/**
Loads the game from memory.
**/
void load(const int state) {
	//printf("DEBUG: child(%i).kill(); ", (int )getpid()); fflush(stdout);
	//printf("DEBUG: parent(%i).continue(); ", (int )getppid()); fflush(stdout);
	printf("*"); fflush(stdout);//a graceful puff of smoke
	printf("<%i poked %i>", (int )getpid(), (int )conf->pids[state]); fflush(stdout);
	kill(conf->pids[state], SIGCONT);
	printf("<%i killed %i>", (int )getpid(), (int )conf->pids[0]); fflush(stdout);
	const int zorg = conf->pids[0];
	conf->pids[0] = 0;
	kill(zorg, SIGKILL);
}

/**
Overloads wgetch with a simple log wrapper.
**/
bool was_meta = FALSE;//not good
int was_colon = FALSE;//worse
struct mvaddnstr_s {
	const int y;
	const int x;
	const char *string;
	const int n;
	struct mvaddnstr_s *next;
};
typedef struct mvaddnstr_s mvaddnstr_t;
mvaddnstr_t *draw_queue;

char codeins[17];
int frame = 0, now = 0;//0x7fe81780
int OVERLOAD(wgetch)(WINDOW *win) {
	printl("Called wgetch.\n");
	int key = real_wgetch(win);
	if (key == 'j') {
		save(1);
		return NULL;
	}
	else if (key == 'J') {
		load(1);
		return NULL;//redundant
	}
	else if (key == 'b') {
		save(2);
		return NULL;
	}
	else if (key == 'B') {
		load(2);
		return NULL;
	}
	if (!was_meta && !was_colon && (key == 0x3a || key == 'w')) was_colon = key == 0x3a ? 1 : 2;//booleans are fun like that
	else if (!was_meta && key == 0x1b) was_meta = TRUE;
	else {
		const char code[5];
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
		frame++;
	}
	//wrefresh(win);
	return key;
}

/**
Overloads time with a simple log wrapper.
**/
time_t OVERLOAD(time)(time_t *timer) {
	printl("Called time.\n");
	return (time_t )now;
}

/**
Overloads wrefresh with a simple log wrapper.
**/
int OVERLOAD(wrefresh)(WINDOW *win) {
	//printl("Called wrefresh.\n");
	int x, y;
	getyx(win, y, x);
	init_pair(16, COLOR_BLACK, COLOR_RED);//TODO find out what pairs are already in use
	init_pair(17, COLOR_BLACK, COLOR_YELLOW);
	init_pair(18, COLOR_BLACK, COLOR_GREEN);
	init_pair(19, COLOR_BLACK, COLOR_CYAN);
	init_pair(20, COLOR_BLACK, COLOR_BLUE);
	init_pair(21, COLOR_BLACK, COLOR_MAGENTA);
	char line[17], liner[17];
	attr_t attrs;
	short pair;
	
	int X = 80, Y = 24;//TODO macros
	wattr_get(win, &attrs, &pair, NULL);

	wattrset(win, COLOR_PAIR(20));
	sprintf(line, "T: 0x%08x", now);
	snprintf(liner, 17, "%-13s", line);
	X -= strlen(liner);
	mvaddnstr(Y, X, liner, 80-X);
	wattrset(win, COLOR_PAIR(19));
	sprintf(line, "R: 0x%02x%02x", ARC4_I, ARC4_J);
	snprintf(liner, 17, "%-9s", line);
	X -= strlen(liner)+1;
	mvaddnstr(Y, X, liner, 80-X);
	wattrset(win, COLOR_PAIR(18));
	sprintf(line, "G: %i", TURN);
	snprintf(liner, 17, "%-13s", line);
	X -= strlen(liner)+1;
	mvaddnstr(Y, X, liner, 80-X);
	wattrset(win, COLOR_PAIR(17));
	sprintf(line, "F: %i", frame);
	snprintf(liner, 17, "%-13s", line);
	X -= strlen(liner)+1;
	mvaddnstr(Y, X, liner, 80-X);
	wattrset(win, COLOR_PAIR(16));
	sprintf(line, "I: %s", codeins);
	snprintf(liner, 17, "%-10s", line);
	X -= strlen(liner)+1;
	mvaddnstr(Y, X, liner, 80-X);

	char some[0x100];//oh look, a hack
	strcpy(some, "Processes: ");
	for (int index = 0; index < SAVE_STATES; index++) {
		if (conf != NULL && conf->pids != NULL) {
			char somer[0x50];
			sprintf(somer, "%s%i:%i", some, index, conf->pids[index]);
			strcpy(some, somer);
			strcat(some, ", ");
		}
	}
	strcat(some, "END\n");
	mvaddnstr(20, 0, some, 80-20);

	wattr_set(win, attrs, pair, NULL);
	wmove(win, y, x);
	int result = real_wrefresh(win);

	return result;
}

/**
Tells sweet lies about the terminal size.
**/
int OVERLOAD(ioctl)(int d, unsigned long request, ...) {
	printl("Called ioctl.\n");
	initialize();//TODO don't assume this is called first even though it is
	va_list	ap;
	va_start(ap, request);
	void *arg = va_arg(ap, void *);
	int result;
	if (request == TIOCGWINSZ) {
		result = real_ioctl(d, request, arg);
		struct winsize *size;
		size = (struct winsize *)arg;
		size->ws_row = 25;
		size->ws_col = 80;
	}
	else result = real_ioctl(d, request, arg);
	va_end(ap);
	return result;
}

#endif
