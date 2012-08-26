/**
@see shm.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef SHM_H
#define SHM_H

#include <sys/types.h>//pid_t

#include <curses.h>//chtype

#include "def.h"//mode_d

/**
Contains pointers to the objects in the shared memory segment.

Pointers are not shared:
<pre>
============== stack

 ,------------ * mode
 |
 | ,---------- * ppid
 | |
 | | ,-------- * pids
 | | |
 | | |     ,-- *** chs
 | | |     |
============== heap
 | | |     |
 | | |     `-> ** chs[0] ------.
 | | |         ** chs[1]       |
 | | |               ...       |
 | | |         ** chs[states] -+--.
 | | |                         |  |
 | | | ,------ * chs[0][0] <---'  |
 | | | |       * chs[0][1]        |
 | | | |               ...        |
 | | | | ,---- * chs[0][rows]     |
 | | | | |          ...           |
 | | | | | ,-- * chs[states][0] <-'
 | | | | | |
============== shm
 | | | | | |
 `-+-+-+-+-+-> mode
   | | | | |
   `-+-+-+-+-> ppid
     | | | |
     `-+-+-+-> pids[0]
       | | |   pids[1]
       | | |       ...
       | | |   pids[states]
       | | |
       `-+-+-> chs[0][0][0]
         | |   chs[0][0][1]
         | |            ...
         | |   chs[0][0][cols]
         | |         ...
         `-+-> chs[0][rows][0]
           |      ...
           `-> chs[states][0][0]
</pre>

@var mode A pointer to the execution mode.
@var ppid A pointer to the process identifier of the parent process.
@var pids A pointer to the process identifiers of the child processes.
@var chs A pointer to the screens of the child processes.
**/
struct shm_s {
	mode_d * mode;
	pid_t * ppid;
	pid_t * pids;
	chtype *** chs;
};
typedef struct shm_s shm_d;

extern shm_d shm;

int uninit_shm(void);
int init_shm(void);
int detach_shm(void);
int attach_shm(void);

#endif
