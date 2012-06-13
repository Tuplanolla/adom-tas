/**
Provides direct access to the executable.
**/
#ifndef ADOM_C
#define ADOM_C

#include <string.h>
#include <curses.h>

#include "util.h"
#include "adom.h"

/*
Simulates the ARC4 of the executable.
*/

/**
The state S.
**/
unsigned char arc4_s[0x100];

/**
The first iterator i.
**/
unsigned char arc4_i = 0x00;

/**
The second iterator j.
**/
unsigned char arc4_j = 0x00;

/**
Returns the hash of a state.

@param s The state S.
@return The hash h.
**/
int harc4(unsigned char * s) {
	const int prime = 0x0000001f;
	int result = 0x00000001;
	unsigned char i = 0x00;
	do {
		result = prime*result+(int )s[i];
		i++;
	} while(i != 0x00);
	return result;
}

/**
Seeds the current state.

@param seed The seed k to seed the state S with.
**/
void sarc4(const int seed) {
	unsigned char i = 0x00, j = 0x00;
	do {
		arc4_s[i] = i;
		i++;
	} while(i != 0x00);
	do {
		j += arc4_s[i]+((unsigned char * )&seed)[(int )i%sizeof (int)];
		SWAP(arc4_s[i], arc4_s[j]);
		i++;
	} while(i != 0x00);
}

/**
Generates a byte (and changes the current state).

The order of operations is wrong to replicate the behavior of the executable.

@return The byte r.
**/
unsigned char arc4() {
	arc4_j += arc4_s[arc4_i];//should be at point A
	SWAP(arc4_s[arc4_i], arc4_s[arc4_j]);//should be at point B
	arc4_i++;
	//point A
	//point B
	return arc4_s[(unsigned char )(arc4_s[arc4_i]+arc4_s[arc4_j])];
}

/*
Returns the key code of a key number.
The key code is three characters long at most.

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
	if (key >= 0x00 && key < 0x1f) key_code_RETURNF("\\C%c", (char )(0x60+key));//control keys
	if (key >= KEY_F(1) && key <= KEY_F(64)) key_code_RETURNF("\\%d", key-KEY_F(0));//function keys
	if (key > 0x20 && key < 0x80) key_code_RETURNF("%c", (char )key);//printable keys
	key_code_RETURN("\\?");//nonprintable keys
}

#endif
