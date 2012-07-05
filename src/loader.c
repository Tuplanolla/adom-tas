/**
Modifies the executable.
**/
#ifndef LOADER_C
#define LOADER_C

#include <stdlib.h>//TODO get rid of the unnecessary
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <dlfcn.h>//dl*
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
//sig*, SA_*, RTLD_*
#include <curses.h>

#include "util.h"
#include "problem.h"
#include "log.h"
#include "def.h"
#include "put.h"
#include "config.h"
#include "asm.h"
#include "shm.h"
#include "exec.h"
#include "record.h"
#include "lib.h"
#include "loader.h"

void * libc_handle;
void * libncurses_handle;

/**
Uninitializes this process.
**/
problem_t uninit_common(void) {
	PROPAGATE(detach_shm());

	PROPAGATE(uninit_config());

	dlclose(libc_handle);
	dlclose(libncurses_handle);

	return NO_PROBLEM;
}

/**
Uninitializes this process.
**/
problem_t uninit_child(void) {
	PROPAGATE(uninit_common());

	/*
	Exits gracefully.
	*/
	um_exit(NO_PROBLEM);

	return NO_PROBLEM;
}

/**
Uninitializes all processes.
**/
problem_t uninit_parent(void) {
	PROPAGATE(uninit_common());

	PROPAGATE(uninit_shm());

	/*
	Exits gracefully.
	*/
	um_exit(NO_PROBLEM);

	return NO_PROBLEM;
}

/**
Initializes this process.
**/
problem_t init_parent(void) {
	init_loader_config();

	/*
	Loads functions from dynamically linked libraries.

	Requires either <code>RTLD_LAZY</code> or <code>RTLD_NOW</code>.
	*/
	libc_handle = dlopen(libc_path, RTLD_LAZY);
	if (libc_handle == NULL) {
		return error(LIBC_DLOPEN_PROBLEM);
	}
	um_unlink = (UNLINK )dlsym(libc_handle, "unlink");
	um_time = (TIME )dlsym(libc_handle, "time");
	um_localtime = (LOCALTIME )dlsym(libc_handle, "localtime");
	um_srandom = (SRANDOM )dlsym(libc_handle, "srandom");
	um_random = (RANDOM )dlsym(libc_handle, "random");
	um_ioctl = (IOCTL )dlsym(libc_handle, "ioctl");
	um_exit = (EXIT )dlsym(libc_handle, "exit");

	libncurses_handle = dlopen(libncurses_path, RTLD_LAZY);
	if (libncurses_handle == NULL) {
		return error(LIBNCURSES_DLOPEN_PROBLEM);
	}
	um_init_pair = (INIT_PAIR )dlsym(libncurses_handle, "init_pair");
	um_wrefresh = (WREFRESH )dlsym(libncurses_handle, "wrefresh");
	um_wgetch = (WGETCH )dlsym(libncurses_handle, "wgetch");

	/*
	Prevents reloading libraries for child processes.
	*/
	if (unsetenv("LD_PRELOAD") != 0) {
		warning(LD_PRELOAD_UNSETENV_PROBLEM);
	}

	/*
	Enables save-quit-load emulation.
	*/
	if (sql) {
		inject_save(&injector);
	}

	/*
	Initializes the shared memory segment.
	*/
	PROPAGATE(init_shm());

	shm.ppid[0] = fork();
	if (shm.ppid[0] == -1) {
		return error(FORK_PROBLEM);
	}
	else if (shm.ppid[0] == 0) {//child
		PROPAGATE(attach_shm());
		shm.pids[0] = getpid();

		signal(SIGWINCH, SIG_IGN);
	}
	else {//parent
		signal(SIGCHLD, SIG_IGN);

		sigset_t mask;
		sigfillset(&mask);
		sigdelset(&mask, SIGINT);//temporary
		sigdelset(&mask, SIGTERM);
		sigsuspend(&mask);

		PROPAGATE(uninit_parent());
	}

	return NO_PROBLEM;
}

#endif
