/**
Provides direct access to the executable.
**/
#ifndef ADOM_C
#define ADOM_C

#include <string.h>

#include "util.h"
#include "adom.h"

/**
Simulates the ARC4 of the executable.
**/
unsigned char arc4_s[0x100];
unsigned char arc4_i = 0x00;
unsigned char arc4_j = 0x00;

/**
Hashes the state <i>S</i>.

@param s The state <i>S</i>.
@return The hash.
**/
int harc4(unsigned char * s) {
	const int prime = 0x0000001f;
	int result = 0x00000001;
	unsigned char i = 0x00;
	do {
		result = prime*result+(int )arc4_s[i];
		i++;
	} while(i != 0x00);
	return result;
}

/**
Seeds the state <i>S</i>.

@param seed The seed.
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
Generates a byte.

@return The byte.
**/
unsigned char arc4() {
	arc4_i++;
	arc4_j += arc4_s[arc4_i];
	SWAP(arc4_s[arc4_i], arc4_s[arc4_j]);
	return arc4_s[(int )(arc4_s[arc4_i]+arc4_s[arc4_j])];
}

#endif
