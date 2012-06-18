/**
Provides direct access to the executable.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef ADOM_H
#define ADOM_H

/**
Points to the turn count of the executable.
**/
#define TURNS ((unsigned int * )0x082b16e0)

/**
Points to the random number generator of the executable.
**/
#define ARC4_S ((unsigned char * )0x082ada40)
#define ARC4_I ((unsigned char * )0x082adb40)
#define ARC4_J ((unsigned char * )0x082adb41)

/**
Lists the amount of random number generator calls measured in the executable.
**/
#define ARC4_CALLS_MENU 4*1214//splash screen and main menu
#define ARC4_CALLS_AUTO_LOAD 4*1419//game after loading it automatically
#define ARC4_CALLS_MANUAL_LOAD 4*1623//game after loading it manually

/**
Lists the minimum and maximum sizes of the terminal.
**/
#define ROWS_MIN 25
#define COLS_MIN 77
#define ROWS_MAX 127
#define COLS_MAX 127

extern unsigned char arc4_s[0x100];
extern unsigned char arc4_i;
extern unsigned char arc4_j;

int harc4();
void sarc4(const int seed);
unsigned char arc4();
void key_code(char * code, const int key);

#endif
