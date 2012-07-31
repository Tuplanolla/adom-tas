/**
@see fork.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef FORK_H
#define FORK_H

#include "prob.h"//problem_d

problem_d save(int state);
problem_d load(int state);

#endif
