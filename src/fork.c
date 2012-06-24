/**
Works like cutlery.
**/
#ifndef FORK_C
#define FORK_C

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

#include "problem.h"//problem_t
#include "shm.h"
#include "fork.h"

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

void continuator() {}

/**
Saves the game to memory.
**/
chtype ** screen;
problem_t save(const int state) {
	int y, x;
	getyx(stdscr, y, x);
	screen = malloc(rows*sizeof (chtype *));
	for (int row = 0; row < rows; row++) {
		chtype * subscreen = malloc(cols*sizeof (chtype));
		for (int col = 0; col < cols; col++) {
			subscreen[col] = mvinch(row, col);
		}
		screen[row] = subscreen;
	}
	fprintfl(warning_stream, "[%d::fork()]", (unsigned short )getpid()); fflush(stdout);
	pid_t pid = fork();//returns 0 in child, process id of child in parent, -1 on error
	attach_shm();
	struct sigaction act;
	act.sa_handler = continuator;
	act.sa_flags = 0;
	if (sigaction(SIGUSR1, &act, NULL) != 0) fprintfl(note_stream, "Can't catch CONT.");
	if (pid == -1) return error(NO_PROBLEM);
	if (pid != 0) {//parent
		if (shm->pids[state] != 0) {
			fprintfl(warning_stream, "[%d::kill(%d)]", (unsigned short )getpid(), (unsigned short )shm->pids[state]); fflush(stdout);
			kill(shm->pids[state], SIGKILL);
		}
		shm->pids[state] = getpid();
		fprintfl(warning_stream, "[%d::stop()]", (unsigned short )getpid()); fflush(stdout);

		sigset_t mask;
		sigfillset(&mask);
		sigdelset(&mask, SIGUSR1);
		sigsuspend(&mask);

		shm->pids[0] = getpid();
		fprintfl(warning_stream, "[%d::continue()]", (unsigned short )getpid()); fflush(stdout);

		for (int row = 0; row < rows; row++) {
			for (int col = 0; col < cols; col++) {
				mvaddch(row, col, screen[row][col]);
			}
			free(screen[row]);
		}
		free(screen);
		wmove(stdscr, y, x);
		wrefresh(stdscr);
	}
	else {//child
		fprintfl(warning_stream, "[%d::inherit(%d)]", (unsigned short )getpid(), (unsigned short )getppid()); fflush(stdout);
		shm->pids[0] = getpid();
	}
	return NO_PROBLEM;
}

/**
Loads the game from memory.
**/
problem_t load(const int state) {
	if (shm->pids[state] != 0) {
		const int killable = shm->pids[0];
		const int continuable = shm->pids[state];
		shm->pids[0] = shm->pids[state];
		shm->pids[state] = 0;
		fprintfl(warning_stream, "[%d::signal(%d)]", (unsigned short )getpid(), (unsigned short )continuable); fflush(stdout);
		kill(continuable, SIGUSR1);
		fprintfl(warning_stream, "[%d::kill(%d)]", (unsigned short )getpid(), (unsigned short )killable); fflush(stdout);

		/*
		Kills two processes for an unknown reason.

		Two wrong outcomes are possible depending on whether SIGKILL is sent.
		<pre>
		P: [11069] [11066] [11067]  00000
		user     11065  0.1  0.0   5244   972 pts/0    S+   17:00   0:00 [adom]        base
		user     11066  0.0  0.1   5456  1788 pts/0    S+   17:00   0:00 [adom]        state 1
		user     11067  0.0  0.1   5488  1044 pts/0    S+   17:00   0:00 [adom]        state 2
		user     11069  0.0  0.0   5488   812 pts/0    S+   17:00   0:00 [adom]        active state
		load 1 [11066]
		P: [11066]  00000  [11067]  00000
		user     11065  0.0  0.0   5244   972 pts/0    S+   17:00   0:00 [adom]        base
		user     11066  0.0  0.0      0     0 pts/0    Z+   17:00   0:00 [adom] <def>  active state (received SIGUSR1 to continue)
		user     11067  0.0  0.1   5488  1044 pts/0    S+   17:00   0:00 [adom]        state 2
		user     11069  0.0  0.0      0     0 pts/0    Z+   17:00   0:00 [adom] <def>  previously active state (received SIGKILL or SIGUSR2 to terminate)

		P: [11069] [11066] [11067]  00000
		user     11065  0.1  0.0   5244   972 pts/0    S+   17:00   0:00 [adom]        base
		user     11066  0.0  0.1   5456  1788 pts/0    S+   17:00   0:00 [adom]        state 1
		user     11067  0.0  0.1   5488  1044 pts/0    S+   17:00   0:00 [adom]        state 2
		user     11069  0.0  0.0   5488   812 pts/0    S+   17:00   0:00 [adom]        active state
		load 1 [11066]
		P: [11066]  00000  [11067]  00000
		user     11065  0.0  0.0   5244   972 pts/0    S+   17:00   0:00 [adom]        base
		user     11066  0.0  0.0      0     0 pts/0    Z+   17:00   0:00 [adom] <def>  active state (received SIGUSR1 to continue)
		user     11067  0.0  0.1   5488  1044 pts/0    S+   17:00   0:00 [adom]        state 2
		user     11069  0.0  0.0   5488   812 pts/0    S+   17:00   0:00 [adom]        previously active state (received nothing)
		</pre>
		*/
		kill(killable, SIGKILL);
	}
	return NO_PROBLEM;
}

#endif
