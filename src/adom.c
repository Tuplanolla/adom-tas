/**
Simulates
	the random number generator and
	the key number interpreter
		of the executable.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef ADOM_C
#define ADOM_C

#include <stdlib.h>//*random
#include <stdio.h>//*print*
#include <string.h>//str*, mem*

#include <curses.h>//KEY_*

#include "util.h"//SWAP
#include "adom.h"//executable_*

/**
The current state S and iterators i and j.
**/
unsigned char arc4_s[0x100];
unsigned char arc4_i = 0x00;
unsigned char arc4_j = 0x00;

/**
Seeds the current state S.

@param seed The seed k.
**/
void sarc4(const int seed) {
	unsigned char i = 0x00, j = 0x00;
	do {
		arc4_s[i] = i;
		i++;
	} while(i != 0x00);
	do {
		j += arc4_s[i] + ((unsigned char * )&seed)[i % sizeof seed];
		SWAP(arc4_s[i], arc4_s[j]);
		i++;
	} while (i != 0x00);
}

/**
Generates a byte r (and changes the current state).

The order of operations is wrong to replicate the behavior of the executable.

@return The byte r.
**/
unsigned char arc4() {
	arc4_j += arc4_s[arc4_i];//should be at point A
	SWAP(arc4_s[arc4_i], arc4_s[arc4_j]);//should be at point B
	arc4_i++;
	//point A
	//point B
	return arc4_s[(unsigned char )(arc4_s[arc4_i] + arc4_s[arc4_j])];
}

/**
Generates and injects
	the state S and
	the iterators i and j.

@param seed The seed k to use.
@param bytes The amount of bytes r to generate.
**/
void iarc4(const int seed, const unsigned int bytes) {
	arc4_i = 0;
	arc4_j = 0;
	srandom(seed);
	sarc4(random());
	for (unsigned int byte = 0; byte < bytes; byte++) {
		arc4();
	}
	memcpy(executable_arc4_s, arc4_s, sizeof arc4_s);
	memcpy(executable_arc4_i, &arc4_i, sizeof arc4_i);
	memcpy(executable_arc4_j, &arc4_j, sizeof arc4_j);
	(*executable_saves)++;
}

/*
Returns the key code of a key number.

The key code is three characters long at most:
<pre>
int key = 42;
char code[4];
key_code(code, key);
printf("%d = \"%s\"\n", key, code);
</pre>

@param code The key code to return.
@param key The key number.
*/
void key_code(char * code, const int key) {
	#define key_code_RETURN(str) {\
			strcpy(code, str);\
			return;\
		}
	#define key_code_RETURNF(format, str) {\
			sprintf(code, format, str);\
			return;\
		}
	if (key == '\\') key_code_RETURN("\\\\");//backslash
	if (key == KEY_UP) key_code_RETURN("\\U");//Up
	if (key == KEY_DOWN) key_code_RETURN("\\D");//Down
	if (key == KEY_LEFT) key_code_RETURN("\\L");//Left
	if (key == KEY_RIGHT) key_code_RETURN("\\R");//Right
	if (key == ' ') key_code_RETURN("\\S");//Space
	if (key == 0x1b) key_code_RETURN("\\M");//Meta (Alt or Esc)
	if (key == 0x7f) key_code_RETURN("\\C_");//Delete
	if (key == KEY_A1) key_code_RETURN("\\H");//keypad Home
	if (key == KEY_A3) key_code_RETURN("\\+");//keypad PageUp
	if (key == KEY_B2) key_code_RETURN("\\.");//keypad center
	if (key == KEY_C1) key_code_RETURN("\\E");//keypad End
	if (key == KEY_C3) key_code_RETURN("\\-");//keypad PageDown
	if (key >= 0x00 && key < 0x1f) key_code_RETURNF("\\C%c", (char )(0x60 + key));//control keys
	if (key >= KEY_F(1) && key <= KEY_F(64)) key_code_RETURNF("\\%d", key - KEY_F(0));//function keys
	if (key > 0x20 && key < 0x80) key_code_RETURNF("%c", (char )key);//printable keys
	key_code_RETURN("\\?");//nonprintable keys
}

#endif
