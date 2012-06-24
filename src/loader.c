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
#include "adom.h"
#include "record.h"
#include "lib.h"

record_t record;

void * libc_handle;
void * libncurses_handle;

/**
Uninitializes this process.

Contains unnecessary checks.
**/
void uninit_child(problem_t code) {
	detach_shm();

	/*
	Closes the log streams.
	*/
	if (error_stream != NULL) fclose(error_stream);
	if (warning_stream != NULL) fclose(warning_stream);
	if (note_stream != NULL) fclose(note_stream);
	if (call_stream != NULL) fclose(call_stream);

	/*
	Exits gracefully.
	*/
	exit(code);
}

/**
Uninitializes all processes.

Contains unnecessary checks.
**/
void uninit_parent(problem_t code) {
	uninit_shm();

	/*
	Closes the log streams.
	*/
	if (error_stream != NULL) fclose(error_stream);
	if (warning_stream != NULL) fclose(warning_stream);
	if (note_stream != NULL) fclose(note_stream);
	if (call_stream != NULL) fclose(call_stream);

	dlclose(libc_handle);//TODO move
	dlclose(libncurses_handle);

	/*
	Exits gracefully.
	*/
	um_exit(code);
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
		uninit_parent(NO_PROBLEM);
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
		uninit_child(NO_PROBLEM);
	}
}

/**
Initializes this process.
**/
problem_t init_parent(void) {
	init_loader_config();

	/*
	Loads functions from dynamically linked libraries.
	*/
	libc_handle = dlopen(libc_path, RTLD_LAZY);//requires either RTLD_LAZY or RTLD_NOW
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
		struct sigaction act;
		act.sa_handler = handle_child;
		act.sa_flags = SA_NOCLDSTOP | SA_NOCLDWAIT;
		if (sigaction(SIGUSR1, &act, NULL) != 0) {
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
