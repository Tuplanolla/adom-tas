/**
Creates,
initializes,
attaches,
detaches and
removes
	the shared memory segment.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef SHM_C
#define SHM_C

#include <stdlib.h>//*alloc, free, ptrdiff_t, NULL
#include <string.h>//str*
#include <sys/ipc.h>//IPC_*
#include <sys/shm.h>//shm*, SHM_*
#include <sys/types.h>//pid_t, key_t

#include <curses.h>//chtype

#include "util.h"//hash, intern, SUBNULL
#include "prob.h"//problem_d, PROPAGATE*, *_PROBLEM
#include "log.h"//error, warning, note
#include "def.h"//project_name
#include "cfg.h"//*

#include "shm.h"

/**
Pointers to the shared memory segment.
**/
intern shm_d shm = {
	.ppid = NULL,
	.pids = NULL,
	.chs = NULL
};

/**
The shared memory key.
**/
key_t key = 0;

/**
The shared memory identifier.
**/
int shmid = -1;

/**
A pointer to the shared memory segment.
**/
void * shmaddr = NULL;

/**
Finds the shared memory segment.

@return The error code.
**/
problem_d get_shm(const int shmflg) {
	/*
	Finds the shared memory segment.
	*/
	const size_t size = sizeof *shm.ppid
			+ (size_t )states * sizeof *shm.pids
			+ (size_t )(states * rows * cols) * sizeof ***shm.chs;
	shmid = shmget(key, size, shmflg);
	if (shmid == -1) {
		return error(SHM_GET_PROBLEM);
	}
	shmaddr = shmat(shmid, NULL, 0);
	if (shmaddr == SUBNULL) {
		return error(SHM_ATTACH_PROBLEM);
	}

	/*
	Calculates the offsets for the pointers to the shared memory segment.

	The segment's location varies between processes.
	*/
	unsigned char * position = (unsigned char * )shmaddr;
	shm.ppid = (pid_t * )position;
	position += (ptrdiff_t )sizeof *shm.ppid;
	shm.pids = (pid_t * )position;
	position += (ptrdiff_t )((size_t )states * sizeof *shm.pids);
	shm.chs = malloc((size_t )states * sizeof *shm.chs);
	if (shm.chs == NULL) {
		return error(SHM_MALLOC_PROBLEM);
	}
	for (int state = 0; state < states; state++) {
		shm.chs[state] = malloc((size_t )rows * sizeof **shm.chs);
		if (shm.chs[state] == NULL) {
			return error(SHM_MALLOC_PROBLEM);
		}
		for (int row = 0; row < rows; row++) {
			shm.chs[state][row] = (chtype * )position;
			position += (ptrdiff_t )((size_t )cols * sizeof ***shm.chs);
		}
	}

	return NO_PROBLEM;
}

/**
Initializes the shared memory segment.

@return The error code.
**/
problem_d init_shm(void) {
	/*
	Generates a key for the shared memory segment.
	*/
	const int proj_id = hash((const unsigned char * )project_name, strlen(project_name));
	key = ftok(shm_path, proj_id);
	if (key == -1) {
		return error(SHM_KEY_PROBLEM);
	}

	/*
	Creates the shared memory segment.
	*/
	PROPAGATE(get_shm(SHM_R | SHM_W | IPC_CREAT));

	/*
	Sets the default values of the objects in the shared memory segment.
	*/
	shm.ppid[0] = (pid_t )0;
	for (int state = 0; state < states; state++) {
		shm.pids[state] = (pid_t )0;
	}
	for (int state = 0; state < states; state++) {
		for (int row = 0; row < rows; row++) {
			for (int col = 0; col < cols; col++) {
				shm.chs[state][row][col] = (chtype )' ';
			}
		}
	}

	return NO_PROBLEM;
}

/**
Attaches the shared memory segment.

@return The error code.
**/
problem_d attach_shm(void) {
	/*
	Accesses the shared memory segment.
	*/
	PROPAGATE(get_shm(SHM_R | SHM_W));

	return NO_PROBLEM;
}

/**
Detaches the shared memory segment.

@return The error code.
**/
problem_d detach_shm(void) {
	shm.ppid = NULL;
	shm.pids = NULL;
	for (int state = 0; state < states; state++) {
		if (shm.chs[state] != NULL) {
			free(shm.chs[state]);
			shm.chs[state] = NULL;
		}
	}
	if (shm.chs != NULL) {
		free(shm.chs);
		shm.chs = NULL;
	}

	if (shmdt(shmaddr) == -1) {
		return error(SHM_DETACH_PROBLEM);
	}
	shmaddr = NULL;

	return NO_PROBLEM;
}

/**
Removes the shared memory segment.

@return The error code.
**/
problem_d uninit_shm(void) {
	if (shmctl(shmid, IPC_RMID, NULL) == -1) {
		return error(SHM_REMOVE_PROBLEM);
	}
	key = 0;
	shmid = -1;

	return NO_PROBLEM;
}

#endif
