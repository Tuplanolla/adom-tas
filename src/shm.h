/**
@file shm.h
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef SHM_H
#define SHM_H

#include <sys/types.h>//pid_t

#include <curses.h>//chtype

/**
Lists the shared states.
**/
enum state_e {
	GETTING_STARTED,
	ALMOST_READY,
	RUNNING,
	HAD_ENOUGH,
	LAST_ONE_STANDING
};
typedef enum state_e state_d;

/**
Contains pointers to the objects in the shared memory segment.

Pointers are not shared:
<pre>
============== stack

 ,------------ * state
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
 `-+-+-+-+-+-> state
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
struct shared_s {
	state_d * state;
	pid_t * ppid;
	pid_t * pids;
	chtype *** chs;
};
typedef struct shared_s shared_d;

extern shared_d shared;

int shm_uninit(void);
int shm_init(void);
int shm_detach(void);
int shm_attach(void);

#endif
