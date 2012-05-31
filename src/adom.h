/**
Provides direct access to the executable.
**/
#ifndef ADOM_H
#define ADOM_H

/**
Points to the turn count of the executable.
**/
#define TURNS (*((unsigned int *)0x082b16e0))

/**
Points to the random number generator's variables of the executable.
**/
#define ARC4_S (*((unsigned char **)0x082ada40))
#define ARC4_I (*((unsigned char *)0x082adb40))
#define ARC4_J (*((unsigned char *)0x082adb41))

/**
Points to the amount of initialized pairs.
**/
#define PAIRS 32

#endif
