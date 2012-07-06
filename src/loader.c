/**
Does something unnecessary.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef LOADER_C
#define LOADER_C

#include <unistd.h>//fork
#include <signal.h>//sig*, RTLD_*

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

/**
Uninitializes this process.
**/
problem_t uninit_common(void) {
	PROPAGATE(detach_shm());

	PROPAGATE(uninit_config());

	return NO_PROBLEM;
}

/**
Uninitializes this process.

@param problem The error code to return.
**/
problem_t uninit_child(const problem_t problem) {
	PROPAGATE(uninit_lib());

	/*
	Exits gracefully.
	*/
	um_exit(problem);

	return NO_PROBLEM;
}

/**
Uninitializes all processes.

@param problem The error code to return.
**/
problem_t uninit_parent(const problem_t problem) {
	PROPAGATE(uninit_lib());

	PROPAGATE(uninit_shm());

	/*
	Exits gracefully.
	*/
	um_exit(problem);

	return NO_PROBLEM;
}

/**
Initializes this process.
**/
problem_t init_parent(void) {
	PROPAGATE(init_loader_config());

	PROPAGATE(init_lib());

	/*
	Enables save-quit-load emulation.
	*/
	if (sql) {
		inject_save(&save_quit_load);
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

		PROPAGATE(uninit_parent(NO_PROBLEM));
	}

	return NO_PROBLEM;
}

#endif
