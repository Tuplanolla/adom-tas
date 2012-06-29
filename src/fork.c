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

/**
Saves the game to memory.
**/
problem_t save(const int state) {
	int y, x;
	attr_t attrs; attr_t * _attrs = &attrs;
	short pair; short * _pair = &pair;
	getyx(stdscr, y, x);
	wattr_get(stdscr, _attrs, _pair, NULL);
	/*for (unsigned int row = 0; row < rows; row++) {
		for (unsigned int col = 0; col < cols; col++) {
			shm->chs[state][row][col] = mvinch((int )row, (int )col);
		}
	}*/
	signal(SIGCHLD, SIG_IGN);//just in case
	fprintfl(error_stream, "[fork]");
	pid_t pid = fork();//returns 0 in child, process id of child in parent, -1 on error
	if (pid == -1) {
		return error(FORK_PROBLEM);
	}
	else if (pid != 0) {//parent
		attach_shm();

		if (get_shm_pid(state) != 0) {
			fprintfl(error_stream, "[displace -> %d]", (unsigned short )get_shm_pid(state));
			kill(get_shm_pid(state), SIGKILL);
		}
		set_shm_pid(state, getpid());
		fprintfl(error_stream, "[stop]");
		set_shm_pid(0, pid);

		struct timespec req;
		req.tv_sec = 0;
		req.tv_nsec = 1000000000l/16;
		while (get_shm_pid(0) != getpid()) {
			nanosleep(&req, NULL);
		}

		fprintfl(error_stream, "[continue]");

		/*for (unsigned int row = 0; row < rows; row++) {
			for (unsigned int col = 0; col < cols; col++) {
				mvaddch((int )row, (int )col, shm->chs[state][row][col]);
			}
		}
		wattr_set(stdscr, attrs, pair, NULL);
		wmove(stdscr, y, x);
		wrefresh(stdscr);*/
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
problem_t load(const int state) {
	if (get_shm_pid(state) != 0) {
		const int killable = get_shm_pid(0);
		const int continuable = get_shm_pid(state);
		set_shm_pid(0, continuable);
		set_shm_pid(state, 0);
		fprintfl(error_stream, "[poke -> %d]", (unsigned short )continuable);
		fprintfl(error_stream, "[die -> %d]", (unsigned short )killable);
		kill(killable, SIGKILL);
		//uninit_child();
	}
	return NO_PROBLEM;
}

#endif
