/**
Modifies the executable.
**/
#ifndef LOADER_H
#define LOADER_H

#include "problem.h"//problem_t

void handle_parent(int sig);
void handle_child(int sig);
problem_t init_parent(void);
problem_t uninit_child(void);
problem_t uninit_parent(void);

#endif
