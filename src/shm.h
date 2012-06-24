/**
Handles the shared memory segment.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef SHM_H
#define SHM_H

#include <sys/types.h>//pid_t

#include <curses.h>//chtype

#include "problem.h"//problem_t

/**
Contains the objects in the shared memory segment.

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
extern shm_t * shm;

/**
Manages the shared memory segment.
**/
problem_t init_shm(void);
problem_t attach_shm(void);
problem_t detach_shm(void);
problem_t uninit_shm(void);

#endif
