/**
Creates,
 initializes,
 attaches,
 detaches and
 removes
  the shared memory segment.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#include <stdlib.h>//*alloc, free, ptrdiff_t, NULL
#include <string.h>//str*
#include <sys/ipc.h>//IPC_*
#include <sys/shm.h>//shm*, SHM_*
#include <sys/types.h>//pid_t, key_t

#include <curses.h>//chtype

#include "util.h"//hash, SUBNULL, SHM_RW
#include "prob.h"//probno, *_PROBLEM
#include "log.h"//log_*
#include "cfg.h"//cfg_*
#include "proj.h"//project_name

#include "shm.h"

/**
Pointers to the shared memory segment.
**/
shared_d shared = {
	.state = NULL,
	.ppid = NULL,
	.pids = NULL,
	.chs = NULL
};

/**
The shared memory key.
**/
static key_t key;
/**
The shared memory size.
**/
static size_t size;
/**
The shared memory identifier.
**/
static int shmid;
/**
A pointer to the beginning of the shared memory.
**/
static void * shmaddr;

/**
Manages the shared memory segment.

@param create Whether the segment should be created.
@return 0 if successful and -1 otherwise.
**/
static int shm_get(const bool create) {
	/*
	Creates or finds the shared memory segment.
	*/
	int shmflg = SHM_R | SHM_W;
	if (create) {
		shmflg |= IPC_CREAT;
		if (shmget(key, size, shmflg | IPC_EXCL) == -1) {
			probno = log_error(SHM_EXCL_PROBLEM);
			return -1;
		}
	}
	shmid = shmget(key, size, shmflg);
	if (shmid == -1) {
		probno = log_error(SHM_GET_PROBLEM);
		return -1;
	}
	shmaddr = shmat(shmid, NULL, SHM_RW);
	if (shmaddr == SUBNULL) {
		probno = log_error(SHM_ATTACH_PROBLEM);
		return -1;
	}

	/*
	Calculates the offsets for the pointers to the shared memory segment.

	The segment's location varies between processes.
	*/
	unsigned char * position = (unsigned char * )shmaddr;
	shared.state = (state_d * )position;
	position += (ptrdiff_t )sizeof *shared.state;
	shared.ppid = (pid_t * )position;
	position += (ptrdiff_t )sizeof *shared.ppid;
	shared.pids = (pid_t * )position;
	position += (ptrdiff_t )((size_t )cfg_saves * sizeof *shared.pids);
	shared.chs = malloc((size_t )cfg_saves * sizeof *shared.chs);
	if (shared.chs == NULL) {
		probno = log_error(SHM_MALLOC_PROBLEM);
		return -1;
	}
	for (int save = 0; save < cfg_saves; save++) {
		shared.chs[save] = malloc((size_t )cfg_rows * sizeof **shared.chs);
		if (shared.chs[save] == NULL) {
			probno = log_error(SHM_MALLOC_PROBLEM);
			return -1;
		}
		for (int row = 0; row < cfg_rows; row++) {
			shared.chs[save][row] = (chtype * )position;
			position += (ptrdiff_t )((size_t )cfg_cols * sizeof ***shared.chs);
		}
	}

	return 0;
}

/**
Removes the shared memory segment.

The segment needs to be detached before removal.

@return 0 if successful and -1 otherwise.
**/
int shm_uninit(void) {
	/*
	Removes the shared memory segment.
	*/
	if (shmctl(shmid, IPC_RMID, NULL) == -1) {
		probno = log_error(SHM_REMOVE_PROBLEM);
		return -1;
	}
	key = 0;
	shmid = -1;

	return 0;
}

/**
Initializes the shared memory segment.

The segment needs to be attached before use.
The current implementation doesn't require it, but
 future versions might.

@return 0 if successful and -1 otherwise.
**/
int shm_init(void) {
	/*
	Generates a key for the shared memory segment.
	*/
	const int proj_id = hash((const unsigned char * )project_name, strlen(project_name));
	key = ftok(cfg_shm_path, proj_id);
	if (key == -1) {
		probno = log_error(SHM_KEY_PROBLEM);
		return -1;
	}

	/*
	Calculates the size of the shared memory segment.
	*/
	size = sizeof *shared.state
			+ sizeof *shared.ppid
			+ (size_t )cfg_saves * sizeof *shared.pids
			+ (size_t )(cfg_saves * cfg_rows * cfg_cols) * sizeof ***shared.chs;

	/*
	Creates the shared memory segment.
	*/
	if (shm_get(TRUE) == -1) {
		return -1;
	}

	/*
	Sets the default values of the objects in the shared memory segment.
	*/
	*shared.state = (state_d )0;
	*shared.ppid = (pid_t )0;
	for (int save = 0; save < cfg_saves; save++) {
		shared.pids[save] = (pid_t )0;
	}
	for (int save = 0; save < cfg_saves; save++) {
		for (int row = 0; row < cfg_rows; row++) {
			for (int col = 0; col < cfg_cols; col++) {
				shared.chs[save][row][col] = (chtype )' ';
			}
		}
	}

	return 0;
}

/**
Detaches the shared memory segment.

@return 0 if successful and -1 otherwise.
**/
int shm_detach(void) {
	/*
	Clears the pointers to the shared memory segment.
	*/
	shared.state = NULL;
	shared.ppid = NULL;
	shared.pids = NULL;
	if (shared.chs != NULL) {
		for (int save = 0; save < cfg_saves; save++) {
			if (shared.chs[save] != NULL) {
				free(shared.chs[save]);
				shared.chs[save] = NULL;
			}
		}
		free(shared.chs);
		shared.chs = NULL;
	}

	/*
	Detaches the shared memory segment.
	*/
	if (shmdt(shmaddr) == -1) {
		probno = log_error(SHM_DETACH_PROBLEM);
		return -1;
	}
	shmaddr = NULL;

	return 0;
}

/**
Attaches the shared memory segment.

@return 0 if successful and -1 otherwise.
**/
int shm_attach(void) {
	/*
	Attaches the shared memory segment.
	*/
	return shm_get(FALSE);
}
