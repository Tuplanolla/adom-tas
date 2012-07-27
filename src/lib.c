/**
Does something unnecessary.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef LIB_C
#define LIB_C

#include <stdlib.h>//exit
#include <unistd.h>//fork
#include <signal.h>//sig*, RTLD_*

#include "prob.h"//problem_t, PROPAGATE
#include "log.h"//error, warning, note
#include "cfg.h"//*
#include "asm.h"//inject_*
#include "shm.h"//*_shm, shm
#include "rec.h"//record
#include "fcn.h"

#include "lib.h"

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
	PROPAGATE(uninit_fcn());

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
	PROPAGATE(uninit_fcn());

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

	PROPAGATE(init_fcn());

	//PROPAGATE(init_interface());//TODO move

	/*
	Enables save-quit-load emulation.
	*/
	if (sql_emulation) {
		inject_save(&save_quit_load);
	}
	else {
		//inject_save(&nothing);
	}

	/*
	Initializes the shared memory segment.
	*/
	PROPAGATE(init_shm());

	record.timestamp = timestamp;

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
