/**
Modifies the executable.

SIGUSR1 forks.
SIGUSR2 dumps.
SIGTERM terminates.
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
#include <libconfig.h>

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

intern UNLINK um_unlink = NULL;
intern IOCTL um_ioctl = NULL;
intern TIME um_time = NULL;
intern LOCALTIME um_localtime = NULL;
intern SRANDOM um_srandom = NULL;
intern RANDOM um_random = NULL;
intern INIT_PAIR um_init_pair = NULL;
intern WREFRESH um_wrefresh = NULL;
intern WGETCH um_wgetch = NULL;
intern EXIT um_exit = NULL;

intern shm_t * shm;

record_t record;

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
Catches signals or something.
**/
void handle_parent(const int sig) {
	if (sig == SIGUSR1) {
		fprintfl(warning_stream, "#%u caught USR1.", getpid());
	}
	else if (sig == SIGUSR2) {
		fprintfl(warning_stream, "#%u caught USR2.", getpid());
	}
	else if (sig == SIGTERM) {
		fprintfl(warning_stream, "#%u caught TERM.", getpid());
		uninit_parent();
	}
}

/**
Catches signals or something.
**/
void handle_child(const int sig) {
	if (sig == SIGUSR1) {
		fprintfl(warning_stream, "#%u caught USR1.", getpid());
	}
	else if (sig == SIGUSR2) {
		fprintfl(warning_stream, "#%u caught USR2.", getpid());
	}
	else if (sig == SIGTERM) {
		fprintfl(warning_stream, "#%u caught TERM.", getpid());
		uninit_child();
	}
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

	init_record(&record);

	/*
	Injects Assembly instructions to disable the save function of the executable.
	*/
	//inject_save(&injector);

	/*if (signal(SIGWINCH, dreamcatcher) == SIG_ERR) fprintfl(note_stream, "No no resizing!");
	if (signal(SIGCONT, terminator) == SIG_ERR) fprintfl(note_stream, "Can't catch CONT.");
	if (signal(SIGINT, terminator) == SIG_ERR) fprintfl(note_stream, "Can't stop!");
	if (signal(SIGTERM, terminator) == SIG_ERR) fprintfl(note_stream, "Can't catch anything.");*/

	init_shm();

	struct sigaction act;
	act.sa_handler = handle_parent;
	act.sa_flags = 0;
	if (sigaction(SIGTERM, &act, NULL) != 0) {
		fprintfl(error_stream, "Can't catch TERM.");
	}

	pid_t pid = fork();
	if (pid == -1) {
		error(FORK_PROBLEM);
	}
	else if (pid == 0) {//child
		attach_shm();
		shm->pids[0] = getpid();
	}
	else {//parent
		shm->ppid = getpid();
		struct sigaction act_;
		act_.sa_handler = handle_child;
		act_.sa_flags = SA_NOCLDSTOP | SA_NOCLDWAIT;
		if (sigaction(SIGUSR1, &act_, NULL) != 0) {
			fprintfl(note_stream, "Can't catch CONT.");
		}
		if (sigaction(SIGUSR2, &act, NULL) != 0) {
			fprintfl(note_stream, "Can't catch CONT.");
		}
		fprintf(stderr, "The parent process seems to have fallen asleep. Use Ctrl C to wake it up.\n");
		sigset_t mask;
		sigfillset(&mask);
		sigdelset(&mask, SIGUSR1);
		int sig;
		sigwait(&mask, &sig);
		fprintf(stderr, "Quitting...\n");
		return error(NO_PROBLEM/*_MATE*/);
	}

	return NO_PROBLEM;
}

#endif
