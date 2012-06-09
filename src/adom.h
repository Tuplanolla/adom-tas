/**
Provides direct access to the executable.
**/
#ifndef ADOM_H
#define ADOM_H

/**
Points to the turn count of the executable.
**/
#define TURNS (* ((unsigned int * )0x082b16e0))

/**
Points to the random number generator of the executable.
**/
#define ARC4_S ((unsigned char * )0x082ada40)
#define ARC4_I ((unsigned char * )0x082adb40)
#define ARC4_J ((unsigned char * )0x082adb41)
#define SARC4(seed) (((void ( *)(int) )0x08125ea0)(seed))
#define SARC4_TIME() (((void ( *)() )0x08125d10)())//srandom(time(NULL)); SARC4(random());

/**
Points to the amount of initialized pairs.
**/
#define PAIRS 32//TODO automate

extern unsigned char arc4_s[0x100];
extern unsigned char arc4_i;
extern unsigned char arc4_j;

int harc4();
void sarc4(const int seed);
unsigned char arc4();
void key_code(char * code, const int key);

#endif
