/**
Modifies the executable.

SIGUSR1 forks.
SIGUSR2 dumps.
SIGTERM terminates.
**/
#ifndef LOADER_H
#define LOADER_H

#include <stdlib.h>//TODO get rid of the unnecessary
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <dlfcn.h>//dl*
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include <curses.h>
#include <libconfig.h>

void uninit_child(problem_t code);
void uninit_parent(problem_t code);
void handle_parent(const int sig);
void handle_child(const int sig);
problem_t init_parent(void);

#endif
