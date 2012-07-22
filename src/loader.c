/**
Does something unnecessary.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef LOADER_C
#define LOADER_C

#include <stdlib.h>//exit
#include <unistd.h>//fork
#include <signal.h>//sig*, RTLD_*

#include "util.h"
#include "problem.h"
#include "log.h"
#include "def.h"
#include "put.h"
#include "config.h"
#include "asm.h"
#include "interface.h"
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
	exit(problem);

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
	exit(problem);

	return NO_PROBLEM;
}

/**
Initializes this process.
**/
problem_t init_parent(void) {
	PROPAGATE(init_internal_config());

	PROPAGATE(init_lib());

	PROPAGATE(init_interface());

	/*
	Enables save-quit-load emulation.
	*/
	if (sql) {
		inject_save(&save_quit_load);
	}
	else {
		//inject_save(&nothing);
	}

	/*
	Initializes the shared memory segment.
	*/
	PROPAGATE(init_shm());

	const pid_t pid = 0;//fork();//disabled to make debugging easier
	if (pid == -1) {
		return error(FORK_PROBLEM);
	}
	else if (pid == 0) {//child
		PROPAGATE(attach_shm());
		shm.ppid[0] = getppid();
		shm.pids[0] = getpid();
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
