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

#include <stddef.h>//NULL
#include <string.h>//str*
#include <sys/ipc.h>//IPC_*
#include <sys/shm.h>//shm*, SHM_*
#include <sys/types.h>//key_t

#include <curses.h>//chtype

#include "util.h"//hash, intern, SUBNULL
#include "problem.h"//problem_t, PROPAGATE, *_PROBLEM
#include "log.h"//error
#include "def.h"//project_name
#include "config.h"//states, rows, cols, shm_path
#include "shm.h"//shm_t, shm

intern unsigned int states;
intern unsigned int rows;
intern unsigned int cols;
intern char * shm_path;

/**
Contains the objects in the shared memory segment.

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
      ** chs[1] -----+--.
            ...      |  |
      ** chs[states] |  |
,---- * chs[0][0] <--'  |
|  ,- * chs[0][1]       |
|  |          ...       |
|  |  * chs[0][rows]    |
|  |       ...          |
`--+--> chs[0][0][0]    |
   |    chs[0][0][1]    |
   |             ...    |
   |    chs[0][0][cols] |
   `--------> ...       |
           ... <--------'
</pre>

@var ppid The process identifier of the parent process.
@var pids A pointer to the process identifiers of the child processes.
@var chs A pointer to the screens of the child processes.
**/
struct shm_s {
	pid_t ppid;
	pid_t * pids;
	chtype *** chs;
};
typedef struct shm_s shm_t;

/**
The objects in the shared memory segment.
**/
shm_t * shm = NULL;

key_t key = 0;
int shmid = -1;

/**
Finds the shared memory segment.

@return The error code.
**/
problem_t get_shm(const int shmflg) {
	const size_t size = sizeof *shm
			+ states * sizeof *shm->pids
			+ states * sizeof *shm->chs
			+ states * rows * sizeof **shm->chs
			+ states * rows * cols * sizeof ***shm->chs;
	shmid = shmget(key, size, shmflg);
	if (shmid == -1) {
		return error(SHM_GET_PROBLEM);
	}
	shm = shmat(shmid, NULL, 0);
	if (shm == SUBNULL) {
		return error(SHM_ATTACH_PROBLEM);
	}

	return NO_PROBLEM;
}

/**
Calculates the offsets for the pointers in the shared memory segment.

@return The error code.
**/
void offset_shm(void) {
	shm->pids = (int * )((size_t )shm
			+ sizeof *shm);
	shm->chs = (chtype *** )((size_t )shm->pids
			+ states * sizeof *shm->pids);
	for (unsigned int state = 0; state < states; state++) {
		shm->chs[state] = (chtype ** )((size_t )shm->chs
				+ states * sizeof *shm->chs
				+ state * rows * sizeof **shm->chs
				+ state * rows * cols * sizeof ***shm->chs);
		for (unsigned int row = 0; row < rows; row++) {
			shm->chs[state][row] = (chtype * )((size_t )shm->chs[state]
					+ rows * sizeof **shm->chs
					+ row * cols * sizeof ***shm->chs);
		}
	}
}

/**
Gets the parent process identifier object <code>ppid</code>.

@return The error code.
**/
pid_t get_shm_ppid(void) {
	offset_shm();
	return shm->ppid;
}

/**
Sets the parent process identifier object <code>ppid</code>.

@param ppid The object.
**/
void set_shm_ppid(const pid_t ppid) {
	offset_shm();
	shm->ppid = ppid;
}

/**
Gets a child process identifier object <code>pid</code> from <code>pids</code>.

@param state The location of the object.
@return The error code.
**/
pid_t get_shm_pid(const int state) {
	offset_shm();
	return shm->pids[state];
}

/**
Sets a child process identifier object <code>pid</code> from <code>pids</code>.

@param pid The object.
@param state The location of the object.
**/
void set_shm_pid(const int state, const pid_t pid) {
	offset_shm();
	shm->pids[state] = pid;
}

/**
Gets a character object <code>ch</code> from <code>chs</code>.

@param state The location of the object.
@param row ...
@param col ...
@return The error code.
**/
chtype get_shm_ch(const int state, const int row, const int col) {
	offset_shm();
	return shm->chs[state][row][col];
}

/**
Sets a character object <code>ch</code> in <code>chs</code>.

@param ch The object.
@param state The location of the object.
@param row ...
@param col ...
**/
void set_shm_ch(const int state, const int row, const int col, const chtype ch) {
	offset_shm();
	shm->chs[state][row][col] = ch;
}

/**
Initializes the shared memory segment.

@return The error code.
**/
problem_t init_shm(void) {
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
	offset_shm();
	shm->ppid = 0;
	for (unsigned int state = 0; state < states; state++) {
		shm->pids[state] = (pid_t )0;
	}
	for (unsigned int state = 0; state < states; state++) {
		for (unsigned int row = 0; row < rows; row++) {
			for (unsigned int col = 0; col < cols; col++) {
				shm->chs[state][row][col] = (chtype )' ';
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
