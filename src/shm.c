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
#include "type.h"//mode_d
#include "proj.h"//project_name
#include "cfg.h"//cfg_*
#include "log.h"//log_*

#include "shm.h"

/**
Pointers to the shared memory segment.
**/
shm_d shm = {
	.mode = NULL,
	.ppid = NULL,
	.pids = NULL,
	.chs = NULL
};

/**
The shared memory key.
**/
static key_t key = 0;
/**
The shared memory size.
**/
static size_t size = 0;
/**
The shared memory identifier.
**/
static int shmid = -1;
/**
A pointer to the beginning of the shared memory.
**/
static void * shmaddr = NULL;

/**
Manages the shared memory segment.

@param create Whether the segment should be created.
@return 0 if successful and -1 otherwise.
**/
static int get_shm(const bool create) {
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
	shm.mode = (mode_d * )position;
	position += (ptrdiff_t )sizeof *shm.mode;
	shm.ppid = (pid_t * )position;
	position += (ptrdiff_t )sizeof *shm.ppid;
	shm.pids = (pid_t * )position;
	position += (ptrdiff_t )((size_t )cfg_saves * sizeof *shm.pids);
	shm.chs = malloc((size_t )cfg_saves * sizeof *shm.chs);
	if (shm.chs == NULL) {
		probno = log_error(SHM_MALLOC_PROBLEM);
		return -1;
	}
	for (int save = 0; save < cfg_saves; save++) {
		shm.chs[save] = malloc((size_t )cfg_rows * sizeof **shm.chs);
		if (shm.chs[save] == NULL) {
			probno = log_error(SHM_MALLOC_PROBLEM);
			return -1;
		}
		for (int row = 0; row < cfg_rows; row++) {
			shm.chs[save][row] = (chtype * )position;
			position += (ptrdiff_t )((size_t )cfg_cols * sizeof ***shm.chs);
		}
	}

	return 0;
}

/**
Removes the shared memory segment.

The segment needs to be detached before removal.

@return 0 if successful and -1 otherwise.
**/
int uninit_shm(void) {
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
int init_shm(void) {
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
	size = sizeof *shm.mode
			+ sizeof *shm.ppid
			+ (size_t )cfg_saves * sizeof *shm.pids
			+ (size_t )(cfg_saves * cfg_rows * cfg_cols) * sizeof ***shm.chs;

	/*
	Creates the shared memory segment.
	*/
	if (get_shm(TRUE) == -1) {
		return -1;
	}

	/*
	Sets the default values of the objects in the shared memory segment.
	*/
	*shm.mode = (mode_d )0;
	*shm.ppid = (pid_t )0;
	for (int save = 0; save < cfg_saves; save++) {
		shm.pids[save] = (pid_t )0;
	}
	for (int save = 0; save < cfg_saves; save++) {
		for (int row = 0; row < cfg_rows; row++) {
			for (int col = 0; col < cfg_cols; col++) {
				shm.chs[save][row][col] = (chtype )' ';
			}
		}
	}

	return 0;
}

/**
Detaches the shared memory segment.

@return 0 if successful and -1 otherwise.
**/
int detach_shm(void) {
	/*
	Clears the pointers to the shared memory segment.
	*/
	shm.mode = NULL;
	shm.ppid = NULL;
	shm.pids = NULL;
	if (shm.chs != NULL) {
		for (int save = 0; save < cfg_saves; save++) {
			if (shm.chs[save] != NULL) {
				free(shm.chs[save]);
				shm.chs[save] = NULL;
			}
		}
		free(shm.chs);
		shm.chs = NULL;
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
int attach_shm(void) {
	/*
	Attaches the shared memory segment.
	*/
	return get_shm(FALSE);
}
