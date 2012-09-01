/**
@file arc4.h
@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef ARC4_H
#define ARC4_H

extern unsigned char * const exec_arc4_c;
extern unsigned char * const exec_arc4_s;
extern unsigned char * const exec_arc4_i;
extern unsigned char * const exec_arc4_j;

extern const unsigned int exec_arc4_calls;
extern const unsigned int exec_arc4_calls_automatic_load;
extern const unsigned int exec_arc4_calls_manual_load;

void arc4_inject(unsigned long int seed, unsigned int calls);

#endif
