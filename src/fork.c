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
#include "exec.h"
#include "loader.h"

#include "fork.h"

intern const unsigned int executable_temporary_levels;
intern const unsigned int executable_temporary_parts;

intern char ** executable_temporary_paths;
intern FILE * error_stream;

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

		for (unsigned int level = 0; level < executable_temporary_levels; level++) {
			const unsigned int offset = level * executable_temporary_parts;
			for (unsigned int part = 0; part < executable_temporary_parts; part++) {
				const unsigned int path = offset + part;
				const size_t size = strlen(executable_temporary_paths[path]) + 1
						+ uintlen(state) + 1;
				char * const state_path = malloc(size);
				if (state_path == NULL) {
					error(MALLOC_PROBLEM);
				}
				else {
					snprintf(state_path, size, "%s.%u",
							executable_temporary_paths[path],
							state);
						char buf[1024];//slow and bad
						snprintf(buf, sizeof buf, "cp -u %s %s 1>/dev/null 2>/dev/null", executable_temporary_paths[path], state_path);
						system(buf);//copy(state_path, executable_temporary_paths[path]);
					free(state_path);
				}
			}
		}

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
		for (unsigned int level = 0; level < executable_temporary_levels; level++) {
			const unsigned int offset = level * executable_temporary_parts;
			for (unsigned int part = 0; part < executable_temporary_parts; part++) {
				const unsigned int path = offset + part;
				const size_t size = strlen(executable_temporary_paths[path]) + 1
						+ uintlen(state) + 1;
				char * const state_path = malloc(size);
				if (state_path == NULL) {
					error(MALLOC_PROBLEM);
				}
				else {
					snprintf(state_path, size, "%s.%u",
							executable_temporary_paths[path],
							state);
						char buf[1024];//slower and worse
						snprintf(buf, sizeof buf, "cp -u %s %s 1>/dev/null 2>/dev/null", state_path, executable_temporary_paths[path]);
						system(buf);
					//copy(executable_temporary_paths[path], state_path);
					free(state_path);
				}
			}
		}//dry code

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
