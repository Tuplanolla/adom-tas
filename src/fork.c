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

#include "util.h"
#include "problem.h"//problem_t
#include "log.h"
#include "config.h"
#include "shm.h"

#include "fork.h"

intern char * home_path;
intern char * executable_path;
intern char * executable_data_path;
intern char * executable_process_path;
intern char * executable_version_path;
intern char * executable_count_path;
intern char * executable_keybind_path;
intern char * executable_config_path;
intern char * loader_path;
intern char * libc_path;
intern char * libncurses_path;
intern unsigned int generations;
intern unsigned int states;
intern unsigned int rows;
intern unsigned int cols;
intern char * iterator;
intern FILE * input_stream;
intern FILE ** output_streams;
intern char * shm_path;
intern FILE * error_stream;
intern FILE * warning_stream;
intern FILE * note_stream;
intern FILE * call_stream;

void continuator(const int sig) {
	printf("%d!", sig);
}//TODO everything

/**
Saves the game to memory.

SIGUSR1 forks.
SIGUSR2 dumps.
SIGTERM terminates.
**/
problem_t save(const int state) {
	int y, x;
	attr_t attrs; attr_t * _attrs = &attrs;
	short pair; short * _pair = &pair;
	getyx(stdscr, y, x);
	wattr_get(stdscr, _attrs, _pair, NULL);
	for (unsigned int row = 0; row < rows; row++) {
		for (unsigned int col = 0; col < cols; col++) {
			shm->chs[state][row][col] = mvinch((int )row, (int )col);
		}
	}
	signal(SIGCHLD, SIG_IGN);//just in case
	fprintfl(error_stream, "[fork]"); fflush(stdout);
	pid_t pid = fork();//returns 0 in child, process id of child in parent, -1 on error
	attach_shm();
	struct sigaction act;
	act.sa_handler = continuator;
	act.sa_flags = SA_NOCLDSTOP | SA_NOCLDWAIT;
	if (sigaction(SIGUSR1, &act, NULL) != 0) fprintfl(error_stream, "Can't catch CONT.");
	if (pid == -1) {
		error(FORK_PROBLEM);
	}
	else if (pid != 0) {//parent
		if (shm->pids[state] != 0) {
			fprintfl(error_stream, "[kill -> %d]", (unsigned short )shm->pids[state]); fflush(stdout);
			kill(shm->pids[state], SIGKILL);
		}
		shm->pids[state] = getpid();
		fprintfl(error_stream, "[stop]"); fflush(stdout);

		sigset_t mask;
		sigfillset(&mask);
		sigdelset(&mask, SIGUSR1);
		sigsuspend(&mask);

		shm->pids[0] = getpid();
		fprintfl(error_stream, "[continue]"); fflush(stdout);

		for (unsigned int row = 0; row < rows; row++) {
			for (unsigned int col = 0; col < cols; col++) {
				mvaddch((int )row, (int )col, shm->chs[state][row][col]);
			}
		}
		wattr_set(stdscr, attrs, pair, NULL);
		wmove(stdscr, y, x);
		wrefresh(stdscr);
	}
	else {//child
		fprintfl(error_stream, "[inherit -> %d]", (unsigned short )getppid()); fflush(stdout);
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
		fprintfl(error_stream, "[poke -> %d]", (unsigned short )continuable);
		kill(continuable, SIGUSR1);
		fprintfl(error_stream, "[die -> %d]", (unsigned short )killable);

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

		Luckily signals can be replaced by manual yield based on the shm priorities.
		*/
		kill(killable, SIGKILL);
	}
	return NO_PROBLEM;
}

#endif
