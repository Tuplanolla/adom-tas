/**
Manages processes.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef FORK_C
#define FORK_C

#include <stdlib.h>//TODO get rid of the unnecessary
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#include <curses.h>

#include "util.h"
#include "prob.h"//problem_t
#include "log.h"
#include "cfg.h"
#include "shm.h"
#include "exec.h"
#include "lib.h"

#include "fork.h"

/**
Saves the game to memory.
**/
problem_t save(const int state) {
	pid_t pid;
	beginning: pid = fork();//returns 0 in child, process id of child in parent, -1 on error
	if (pid == -1) {
		return error(FORK_PROBLEM);
	}
	else if (pid == 0) {//child
		attach_shm();

		return NO_PROBLEM;
	}
	else {//parent
		signal(SIGCHLD, SIG_IGN);//prevents defunct processes from appearing

		attach_shm();//TODO why?

		/*
		Stores the position of the cursor.
		*/
		short y, x;
		getyx(stdscr, y, x);

		for (int row = 0; row < rows; row++) {
			for (int col = 0; col < cols; col++) {
				shm.chs[state][row][col] = mvwinch(stdscr, (int )row, (int )col);
			}
		}

		if (shm.pids[state] != 0) {
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
					snprintf(state_path, size, "%s_%u",
							executable_temporary_paths[path],
							state);
					copy(state_path, executable_temporary_paths[path]);
					free(state_path);
				}
			}
		}

		struct timespec req;
		req.tv_sec = 0;
		req.tv_nsec = 1000000000l / 16;
		while (shm.pids[0] != getpid()) {
			nanosleep(&req, NULL);
		}

		fprintfl(error_stream, "[continue]");

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
	return NO_PROBLEM;
}

/**
Loads the game from memory.
**/
problem_t load(const int state) {
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
					snprintf(state_path, size, "%s_%u",
							executable_temporary_paths[path],
							state);
					copy(executable_temporary_paths[path], state_path);
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
