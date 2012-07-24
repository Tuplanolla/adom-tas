/**
@see lib.h
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef LOADER_H
#define LOADER_H

#include "prob.h"//problem_t

problem_t init_parent(void);
problem_t uninit_child(problem_t problem);
problem_t uninit_parent(problem_t problem);

#endif
