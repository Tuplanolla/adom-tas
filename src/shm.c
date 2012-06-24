/**
Creates,
initializes,
attaches,
detaches and
removes
	the shared memory segment.

SHM_GET_PROBLEM
SHM_ATTACH_PROBLEM
SHM_KEY_PROBLEM
SHM_DETACH_PROBLEM
SHM_REMOVE_PROBLEM

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef SHM_C
#define SHM_C

#include <string.h>//str*
#include <sys/ipc.h>//IPC_*
#include <sys/shm.h>//shm*, SHM_*
#include <sys/types.h>//key_t

#include <curses.h>//chtype

#include "def.h"//project_name
#include "config.h"//states, rows, cols, shm_path
#include "util.h"//hash, SUBNULL
#include "problem.h"//problem_t, PROPAGATE, *_PROBLEM
#include "log.h"//error
#include "shm.h"//shm_t

unsigned int states;
unsigned int rows;
unsigned int cols;
char * shm_path;

key_t key = 0;
int shmid = -1;
shm_t * shm = NULL;

/**
Creates the shared memory segment.

The values come after the pointers:
<pre>
     ppid
,--- * pids
| ,- *** chs
`-+--> pids[0]
  |    pids[1]
  |        ...
  |    pids[states]
  `-> ** chs[0] -----.
      ** chs[1]      |
            ...      |
      ** chs[states] |
   ,- * chs[0][0] <--'
   |  * chs[0][1]
   |          ...
   |  * chs[0][rows]
   `--> chs[0][0][0]
        chs[0][0][1]
                 ...
        chs[0][0][cols]
</pre>

@return The error code.
**/
problem_t get_shm(const int shmflg) {
	/*
	Finds the shared memory segment.
	*/
	const size_t size = sizeof *shm
			+ states * sizeof *shm->pids
			+ states * rows * cols * sizeof ***shm->chs;
	shmid = shmget(key, size, shmflg);
	if (shmid == -1) {
		return error(SHM_GET_PROBLEM);
	}
	shm = shmat(shmid, NULL, 0);
	if (shm == SUBNULL) {
		return error(SHM_ATTACH_PROBLEM);
	}

	/*
	Calculates offsets for the pointers in the shared memory segment.
	*/
	shm->pids = (int * )(shm
			+ sizeof *shm);
	shm->chs = (chtype *** )(shm->pids
			+ states*sizeof *shm->pids);
	for (unsigned int state = 0; state < states; state++) {
		shm->chs[state] = (chtype ** )(shm->chs
				+ states*sizeof ***shm->chs
				+ state * rows * sizeof **shm->chs);
		for (unsigned int row = 0; row < rows; row++) {
			shm->chs[state][row] = (chtype * )(shm->chs[state]
					+ row * cols * sizeof (*shm->chs));
		}
	}
	return NO_PROBLEM;
}

/**
Initializes the shared memory segment.

@return The error code.
**/
problem_t init_shm(void) {
	/*
	Generates a key for the shared memory segment.
	*/
	const int proj_id = hash((unsigned char * )project_name, strlen(project_name));
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
	shm->ppid = 0;
	for (unsigned int state = 0; state < states; state++) {
		shm->pids[state] = 0;
	}
	for (unsigned int state = 0; state < states; state++) {
		for (unsigned int row = 0; row < rows; row++) {
			for (unsigned int col = 0; col < cols; col++) {
				shm->chs[state][row][col] = 0;
			}
		}
	}
	return NO_PROBLEM;
}

/**
Attaches the shared memory segment.

@return The error code.
**/
problem_t attach_shm(void) {
	PROPAGATE(get_shm(SHM_R | SHM_W));
	return NO_PROBLEM;
}

/**
Detaches the shared memory segment.

@return The error code.
**/
problem_t detach_shm(void) {
	if (shmdt(shm) == -1) {
		return error(SHM_DETACH_PROBLEM);
	}
	shm = NULL;
	return NO_PROBLEM;
}

/**
Removes the shared memory segment.

@return The error code.
**/
problem_t uninit_shm(void) {
	if (shmctl(shmid, IPC_RMID, NULL) == -1) {
		return error(SHM_REMOVE_PROBLEM);
	}
	key = 0;
	shmid = -1;
	return NO_PROBLEM;
}

#endif
