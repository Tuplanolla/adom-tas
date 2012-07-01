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
#include "loader.h"

#include "fork.h"

intern shm_t shm;

/**
Saves the game to memory.
**/
problem_t save(const unsigned int state) {
	signal(SIGCHLD, SIG_IGN);//just in case
	fprintfl(error_stream, "[fork]");
	pid_t pid = fork();//returns 0 in child, process id of child in parent, -1 on error
	if (pid == -1) {
		return error(FORK_PROBLEM);
	}
	else if (pid != 0) {//parent
		attach_shm();

		int y, x;
		getyx(stdscr, y, x);
		for (unsigned int row = 0; row < rows; row++) {
			for (unsigned int col = 0; col < cols; col++) {
				shm.chs[state][row][col] = mvwinch(stdscr, (int )row, (int )col);
			}
		}

		if (shm.pids[state] != 0) {
			fprintfl(error_stream, "[displace -> %d]", (unsigned short )shm.pids[state]);
			kill(shm.pids[state], SIGKILL);
		}
		shm.pids[state] = getpid();
		fprintfl(error_stream, "[stop]");
		shm.pids[0] = pid;

		struct timespec req;
		req.tv_sec = 0;
		req.tv_nsec = 1000000000l/16;
		while (shm.pids[0] != getpid()) {
			nanosleep(&req, NULL);
		}

		fprintfl(error_stream, "[continue]");

		wclear(stdscr);
		for (unsigned int row = 0; row < rows; row++) {
			for (unsigned int col = 0; col < cols; col++) {
				mvwaddch(stdscr, (int )row, (int )col, shm.chs[state][row][col]);
			}
		}
		wmove(stdscr, y, x);
		wrefresh(stdscr);
		//tail recursion!
	}
	else {//child
		attach_shm();
		fprintfl(error_stream, "[inherit <- %d]", (unsigned short )getppid());
	}
	return NO_PROBLEM;
}

/**
Loads the game from memory.
**/
problem_t load(const unsigned int state) {
	if (shm.pids[state] != 0) {
		const int killable = shm.pids[0];
		const int continuable = shm.pids[state];
		shm.pids[0] = continuable;
		shm.pids[state] = (pid_t )0;
		fprintfl(error_stream, "[poke -> %d]", (unsigned short )continuable);
		fprintfl(error_stream, "[die -> %d]", (unsigned short )killable);
		kill(killable, SIGKILL);
		//uninit_child();
	}
	return NO_PROBLEM;
}

#endif
