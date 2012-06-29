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
Manages the shared memory segment.
**/
problem_t init_shm(void);
problem_t attach_shm(void);
problem_t detach_shm(void);
problem_t uninit_shm(void);

pid_t get_shm_ppid(void);
pid_t get_shm_pid(int state);
chtype get_shm_ch(int state, int row, int col);
void set_shm_ppid(pid_t ppid);
void set_shm_pid(int state, pid_t pid);
void set_shm_ch(int state, int row, int col, chtype ch);

#endif
