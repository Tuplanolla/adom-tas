/**
Provides.

TODO put close(shm_fd); somewhere
TODO put close(log_stream); somewhere
**/
#ifndef LOADER_C
#define LOADER_C

#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "config.h"
#include "util.h"
#include "adom.h"
#include "error.h"
#include "loader.h"

/**
Declares the unmodified versions of the functions that are overloaded.
**/
INIT_PAIR real_init_pair;
WCLEAR real_wclear;
WREFRESH real_wrefresh; 
WMOVE real_wmove;
WADDCH real_waddch;
WADDNSTR real_waddnstr;
WINCH real_winch;
WGETCH real_wgetch;
WGETNSTR real_wgetnstr;
UNLINK real_unlink;
TIME real_time;
RAND real_rand;
SRAND real_srand;
RANDOM real_random;
SRANDOM real_srandom;
IOCTL real_ioctl;

/**
Formats and logs a message.
@param fmt The message format.
@param ... The parameters to format.
@return The amount of characters written.
@deprecated
**/
FILE *log_stream = NULL;
int printfl(const char *fmt, ...) {
	va_list	ap;
	va_start(ap, fmt);
	int result = 0;
	if (log_stream == stdout || log_stream == stderr) {
		result += fprintf(log_stream, "Log: ");
	}
	result += vfprintf(log_stream, fmt, ap);
	fflush(log_stream);
	va_end(ap);
	return result;
}

/**
Formats and logs a message.
@param stream The destination stream.
@param fmt The message format.
@param ... The parameters to format.
@return The amount of characters written.
**/
int vfprintfl(FILE *stream, const char *fmt, va_list ap) {
	int result = 0;
	if (stream == stdout || stream == stderr) {
		result += fprintf(stream, "Log: ");
	}
	result += vfprintf(stream, fmt, ap);
	result += fprintf(stream, "\n");
	return result;
}
int fprintfl(FILE *stream, const char *fmt, ...) {
	va_list	ap;
	va_start(ap, fmt);
	const int result = vfprintfl(stream, fmt, ap);
	va_end(ap);
	return result;
}

/**
Logs the ARC4 status.
**/
int printrl() {
	FILE *random_stream = fopen(RANDOM_LOG_PATH, "w");
	int result = 0;
	if (random_stream != NULL) {
		result += fwrite(ARC4_S, sizeof (unsigned char), 0x100, random_stream);
		result += fwrite(ARC4_I, sizeof (unsigned char), 1, random_stream);
		result += fwrite(ARC4_J, sizeof (unsigned char), 1, random_stream);
	}
	fclose(random_stream);
	return result;
}

/**
Logs the inputs.
**/
int printil() {
	FILE *input_stream = fopen(INPUT_LOG_PATH, "w");
	int result = 0;
	if (input_stream != NULL) {
		frame_t *this_frame = get_first_frame();
		while (this_frame != NULL) {
			result += fwrite(&this_frame->duration, sizeof (int), 1, input_stream);
			result += fwrite(&this_frame->input, sizeof (int), 1, input_stream);
			this_frame = this_frame->next;
		}
	}
	fclose(input_stream);
	return result;
}

/**
Logs an error message and returns its error code.
@param code The error code.
@return The error code.
**/
error_t error(const error_t code) {
	printfl("Error: %s\n", error_message(code));
	return code;
}

/**
Logs a warning message and returns its error code.
@param code The error code.
@return The error code.
**/
error_t warning(const error_t code) {
	printfl("Warning: %s\n", error_message(code));
	return code;
}

//There are too many logs. This requires some thinking.

/**
Very important temporary variables.
**/
int frame = 0, now = 0, globstate = 1;//0x7fe81780

/**
Simulates the ARC4 of the executable.
**/
char arc4_s[0x100];
int harc4() {
	const int prime = 0x0000001f;
	int result = 0x00000001;
	unsigned char i = 0x00;
	do {
		result = prime*result+(int )arc4_s[i];
		if (i == 0xff) break;
		i++;
	} while(i != 0x00);
	return result;
}
void sarc4(const int seed) {
	unsigned char i = 0x00, j = 0x00;
	do {
		arc4_s[i] = i;
		i++;
	} while(i != 0x00);
	do {
		j = j+arc4_s[i]+((unsigned char *)&seed)[i%sizeof (int)];
		SWAP(arc4_s[i], arc4_s[j]);
		if (i == 0xff) break;
		i++;
	} while(i != 0x00);
}

int ARC4_H() {//Don't repeat yourself...
	const int prime = 0x0000001f;
	int result = 0x00000001;
	unsigned char i = 0x00;
	do {
		result = prime*result+(int )ARC4_S[i];
		if (i == 0xff) break;
		i++;
	} while(i != 0x00);
	return result;
}

/*
Returns the key code of a key number.
The key code is from one to three characters long.
@param code The key code to return.
@param key The key number.
*/
void key_code(char *code, const int key) {
	#define key_code_RETURN(str) {\
			strcpy(code, str);\
			return;\
		}
	#define key_code_RETURNF(format, str) {\
			sprintf(code, format, str);\
			return;\
		}
	if (key == '\\') key_code_RETURN("\\\\");//backslash
	if (key == KEY_UP) key_code_RETURN("\\U");//Up
	if (key == KEY_DOWN) key_code_RETURN("\\D");//Down
	if (key == KEY_LEFT) key_code_RETURN("\\L");//Left
	if (key == KEY_RIGHT) key_code_RETURN("\\R");//Right
	if (key == ' ') key_code_RETURN("\\S");//Space
	if (key == 0x1b) key_code_RETURN("\\M");//Meta (Alt or Esc)
	if (key == 0x7f) key_code_RETURN("\\C_");//Delete
	if (key == KEY_A1) key_code_RETURN("\\H");//keypad Home
	if (key == KEY_A3) key_code_RETURN("\\+");//keypad PageUp
	if (key == KEY_B2) key_code_RETURN("\\.");//keypad center
	if (key == KEY_C1) key_code_RETURN("\\E");//keypad End
	if (key == KEY_C3) key_code_RETURN("\\-");//keypad PageDown
	if (key >= 0x00 && key < 0x1f) key_code_RETURNF("\\C%c", (char )(0x60+key));//control keys
	if (key >= KEY_F(1) && key <= KEY_F(64)) key_code_RETURNF("\\%d", key-KEY_F(0));//function keys
	if (key > 0x20 && key < 0x80) key_code_RETURNF("%c", (char )key);//printable keys
	key_code_RETURN("\\?");//nonprintable keys
}

/**
Loads functions from dynamically linked libraries (libc and libncurses).
**/
void load_dynamic_libraries() {
	/*
	Imports functions from the C standard library.
	*/
	char *path = getenv("LIBC_PATH");
	if (path == NULL) path = LIBC_PATH;
	void *handle = dlopen(path, RTLD_LAZY);//requires either RTLD_LAZY or RTLD_NOW
	if (handle == NULL) exit(error(DLOPEN_LIBC_ERROR));
	real_unlink = (UNLINK )dlsym(handle, "unlink");
	real_time = (TIME )dlsym(handle, "time");
	real_rand = (RAND )dlsym(handle, "rand");
	real_srand = (SRAND )dlsym(handle, "srand");
	real_random = (RANDOM )dlsym(handle, "random");
	real_srandom = (SRANDOM )dlsym(handle, "srandom");
	real_ioctl = (IOCTL )dlsym(handle, "ioctl");

	/*
	Imports functions from New Cursor Optimization library.
	*/
	path = getenv("LIBNCURSES_PATH");
	if (path == NULL) path = LIBNCURSES_PATH;
	handle = dlopen(path, RTLD_LAZY);
	if (handle == NULL) exit(error(DLOPEN_LIBNCURSES_ERROR));
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
	unsetenv("LD_PRELOAD");//TODO fix
}

/**
Seeds the ARC4 of the executable.

Seeding can be simulated:
<pre>
real_srandom(time(NULL));
sarc4(real_random());
</pre>
**/
void seed() {
	SARC4_TIME();
}

/**
Shares memory or something.
**/
struct shm_s {
	int term;//if needed to fix detaching
	int pids[SAVE_STATES];
};
typedef struct shm_s shm_t;
int shm_fd;//some handle
shm_t *conf;//shared
void shmup() {
	bool first = TRUE;

	printfl("Started to map shared memory.\n");
	shm_fd = shm_open(SHM_PATH, O_RDWR|O_CREAT|O_EXCL, S_IRUSR|S_IWUSR);
	if (shm_fd < 0) {
		first = FALSE;
	}
	shm_fd = shm_open(SHM_PATH, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
	if (shm_fd < 0) {
		printfl("Failed to map shared memory.\n");
		return;//error here
	}

	ftruncate(shm_fd, (off_t )sizeof (shm_t));

	conf = (shm_t *)mmap(NULL, sizeof (shm_t), PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (conf == MAP_FAILED) {
		printfl("Failed to map shared memory.\n");
		return;//error here
	}

	if (first) {
		for (int index = 0; index < SAVE_STATES; index++) conf->pids[index] = 0;
		printfl("Mapped shared memory.\n");
	}
	else printfl("Remapped shared memory.\n");
}

/**
Guarantees the dynamically linked libraries are only loaded once.
**/
bool initialized = FALSE;
void initialize() {
	if (initialized) return;
	initialized = TRUE;

	load_dynamic_libraries();

	log_stream = fopen(CALL_LOG_PATH, "w");
	if (log_stream == NULL) {
		log_stream = stderr;
		warning(LOG_WARNING);
	}

	real_unlink("/dev/shm/adom-tas");//What is portable code?
	shmup();
	conf->pids[0] = getpid();

	printfl("Logging is disabled for: printf, wrefresh, init_pair, wgetch.\n");
}

bool tired = TRUE;
void continuator(const int signo) {
	if (signo == SIGCONT) {
		printfl("Caught CONT.\n");
		tired = FALSE;
	}
}
void dreamcatcher(const int signo) {
	printfl("Somehow caught \"%s\".\n", strsignal(signo));
}

/**
Saves the game to memory.
**/
void save(const int state) {
	pid_t pid = fork();//returns 0 in child, process id of child in parent, -1 on error
	shmup();
	if (signal(SIGTERM, dreamcatcher) == SIG_ERR) printfl("Can't catch anything.\n");
	if (signal(SIGINT, dreamcatcher) == SIG_ERR) printfl("Can't catch anything.\n");
	if (signal(SIGHUP, dreamcatcher) == SIG_ERR) printfl("Can't catch anything.\n");
	if (signal(SIGQUIT, dreamcatcher) == SIG_ERR) printfl("Can't catch anything.\n");
	if (signal(SIGTRAP, dreamcatcher) == SIG_ERR) printfl("Can't catch anything.\n");
	if (signal(SIGABRT, dreamcatcher) == SIG_ERR) printfl("Can't catch anything.\n");
	if (signal(SIGSTOP, dreamcatcher) == SIG_ERR) printfl("Can't catch anything.\n");;
	if (signal(SIGTTOU, dreamcatcher) == SIG_ERR) printfl("Can't catch anything.\n");
	if (pid != (pid_t )NULL) {//parent
		if (conf->pids[state] != 0) {//removes an old save
			kill(conf->pids[state], SIGKILL);//kills the parent of another process and causes weird problems
		}
		conf->pids[state] = getpid();
		if (signal(SIGCONT, continuator) == SIG_ERR) printfl("Can't catch CONT.\n");
		//printf("<%d fell asleep>", (int )getpid()); fflush(stdout);
		struct timespec req;
		req.tv_sec = (time_t )0;
		req.tv_nsec = 1000000000l/16l;//extern this
		while (tired) nanosleep(&req, NULL);
		//printf("<%d woke up>", (int )getpid()); fflush(stdout);
	}
	else {//child
		//printf("<%d is ready>", (int )getpid()); fflush(stdout);
		conf->pids[0] = getpid();
	}
}

/**
Loads the game from memory.
**/
void load(const int state) {
	//printf("<%d poked %d>", (int )getpid(), (int )conf->pids[state]); fflush(stdout);
	if (conf->pids[state] != 0) {
		kill(conf->pids[state], SIGCONT);
		//printf("<%d killed %d>", (int )getpid(), (int )conf->pids[0]); fflush(stdout);
		const int zorg = conf->pids[0];
		conf->pids[0] = conf->pids[state];
		conf->pids[state] = 0;
		kill(zorg, SIGKILL);
	}
}

/**
Annotates (and more) overloaded functions.
**/
#define OVERLOAD initialize();

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
mvaddnstr_t *draw_queue;//never used

bool condensed = FALSE;
char codeins[7];
int wgetch(WINDOW *win) { OVERLOAD//bloat
	//printfl("Called wgetch(0x%08x).\n", (unsigned int )win);
	int key = real_wgetch(win);
	if (key == 0x110) {
		condensed = !condensed;
		wrefresh(win);
		return 0;
	}
	else if (key == 0x111) {
		save(globstate);
		wrefresh(win);
		return 0;
	}
	else if (key == 0x112) {
		load(globstate);
		wrefresh(win);
		return 0;//redundant
	}
	else if (key == 0x113) {
		globstate = globstate%9+1;
		wrefresh(win);
		printrl();//TODO move
		printil();//TODO move
		return 0;
	}
	else if (key == 0x114) {
		now++;
		wrefresh(win);
		return 0;
	}
	else if (key == 'S') {//bad
		seed();//TODO fix
		frame_add(TRUE, 0, 0, now);
		wrefresh(win);
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
		frame++;
	}
	unsigned int duration = 1;
	frame_add(FALSE, duration, key, 0);//meta, colon and w are undisplayed but still recorded (for now)
	//wrefresh(win);
	return key;
}

/**
Overloads rand with a simple log wrapper.
**/
int rand() { OVERLOAD
	printfl("Called rand().\n");
	return real_rand();
}

/**
Overloads srand with a simple log wrapper.
**/
void srand(unsigned int seed) { OVERLOAD
	printfl("Called srand(%u).\n", seed);
	real_srand(seed);
}

/**
Overloads random with a simple log wrapper.
**/
long random() { OVERLOAD
	printfl("Called random().\n");
	return real_random();
}

/**
Overloads srandom with a simple log wrapper.
**/
void srandom(unsigned int seed) { OVERLOAD
	printfl("Called srandom(%u).\n", seed);
	real_srandom(seed);
}

/**
Overloads init_pair with a simple log wrapper.
**/
int init_pair(short pair, short f, short b) { OVERLOAD
	//printfl("Called init_pair(%d, %d, %d).\n", pair, f, b);
	return real_init_pair(pair, f, b);
}

/**
Overloads time with a simple log wrapper.
**/
time_t time(time_t *t) { OVERLOAD
	printfl("Called time(0x%08x).\n", (unsigned int )t);
	const time_t n = (time_t )now;
	if (t != NULL) *t = n;
	return n;
}

/**
Overloads wrefresh with a "simple log wrapper".
**/
int wrefresh(WINDOW *win) { OVERLOAD
	//printfl("Called wrefresh(0x%08x).\n", (unsigned int )win);

	int x, y;
	attr_t attrs;
	short pair;
	getyx(win, y, x);
	wattr_get(win, &attrs, &pair, NULL);

	short ws_pair = PAIRS;
	#define ws_INIT_PAIR(b) \
		real_init_pair(ws_pair, COLOR_BLACK, b);\
		ws_pair++;
	ws_INIT_PAIR(COLOR_RED);
	ws_INIT_PAIR(COLOR_YELLOW);
	ws_INIT_PAIR(COLOR_GREEN);
	ws_INIT_PAIR(COLOR_CYAN);
	ws_INIT_PAIR(COLOR_BLUE);
	ws_INIT_PAIR(COLOR_MAGENTA);

	int ws_x = TERM_COL, ws_y = TERM_ROW-1;
	char ws_str[TERM_COL], ws_buf[TERM_COL];
	#define ws_ADDSTR(format, length, arg) \
		ws_pair--;\
		wattrset(win, COLOR_PAIR(ws_pair));\
		snprintf(ws_str, (size_t )TERM_COL, format, arg);\
		snprintf(ws_buf, (size_t )((condensed ? 1 : length)+1), "%-*s", length, ws_str);\
		ws_x -= length;\
		mvaddnstr(ws_y, ws_x, ws_buf, TERM_COL-ws_x);\
		ws_x--;
	ws_ADDSTR("S: %d", 4, globstate);
	ws_ADDSTR("T: 0x%08x", 13, now);
	ws_ADDSTR("R: 0x%08x", 13, ARC4_H());
	ws_ADDSTR("G: %d", 13, TURNS);
	ws_ADDSTR("F: %d", 13, frame);
	ws_ADDSTR("I: %s", 9, codeins);

	char some[TERM_COL];//a hack
	strcpy(some, "P:");
	for (int index = 0; index < SAVE_STATES; index++) {
		if (conf->pids != NULL) {
			char somer[TERM_COL];
			bool somery = conf->pids[index] != 0;
			sprintf(somer, "%s %c%d%c", some, somery ? '[' : ' ', index, /*conf->pids[index],*/ somery ? ']' : ' ');
			strcpy(some, somer);
		}
	}
	mvaddnstr(21, 10, some, TERM_COL-20);

	wattr_set(win, attrs, pair, NULL);
	wmove(win, y, x);
	const int result = real_wrefresh(win);

	return result;
}

/**
Tells sweet lies about the terminal size.
**/
int ioctl(int d, unsigned long request, ...) { OVERLOAD
	va_list	argp;
	va_start(argp, request);
	const void *arg = va_arg(argp, void *);
	printfl("Called ioctl(%d, %lu, 0x%08x).\n", d, request, (unsigned int )arg);
	const int result = real_ioctl(d, request, arg);
	if (request == TIOCGWINSZ) {
		struct winsize *size;
		size = (struct winsize *)arg;
		size->ws_row = TERM_ROW;
		size->ws_col = TERM_COL;
	}
	va_end(argp);
	return result;
}

#endif
