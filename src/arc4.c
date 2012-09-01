/**
Emulates
 the random number generator
  of the executable.

@file arc4.c
@author Sampsa "Tuplanolla" Kiiskinen
**/
#include <stdlib.h>//*random, size_t
#include <string.h>//mem*
#include <limits.h>//CHAR_BIT

#include "util.h"//SWAP

/**
The random number generator's counter c.
**/
unsigned char * const exec_arc4_c = (void * )0x08264a60;
/**
The random number generator's state S.
**/
unsigned char * const exec_arc4_s = (void * )0x082ada40;
/**
The random number generator's first iterator i.
**/
unsigned char * const exec_arc4_i = (void * )0x082adb40;
/**
The random number generator's second iterator j.
**/
unsigned char * const exec_arc4_j = (void * )0x082adb41;

/**
The amount of random number generator calls measured
 from seeding the random number generator
 to its first cyclic point.
**/
const unsigned int exec_arc4_calls = 1165;
/**
The amount of random number generator calls
 from the splash screen or the main menu
 to loading a game automatically.
**/
const unsigned int exec_arc4_calls_automatic_load = 205;
/**
The amount of random number generator calls measured
 from the splash screen or the main menu
 to loading a game manually (from a list of one game).
**/
const unsigned int exec_arc4_calls_manual_load = 409;

/**
The emulated random number generator's counter c.

Emulates an internal variable:
<pre>
unsigned int * const arc4_calls = (void * )0x08264a60;
</pre>
**/
static unsigned int arc4_c = 0;
/**
The emulated random number generator's state S.

Emulates an internal variable:
<pre>
unsigned char * const arc4_state = (void * )0x082ada40;
</pre>
**/
static unsigned char arc4_s[256];
/**
The emulated random number generator's first iterator i.

Emulates an internal variable:
<pre>
unsigned char * const first_arc4_iterator = (void * )0x082adb40;
</pre>
**/
static unsigned char arc4_i = 0;
/**
The emulated random number generator's second iterator j.

Emulates an internal variable:
<pre>
unsigned char * const second_arc4_iterator = (void * )0x082adb41;
</pre>
**/
static unsigned char arc4_j = 0;

/**
Seeds the current state S with the seed k.

Emulates an internal function:
<pre>
void (* const seed_arc4)(unsigned long int seed) = (void * )0x08125ea0;
</pre>

@param k The seed k.
**/
static void sarc4(const unsigned long int k) {
	unsigned char i = 0;
	unsigned char j = 0;
	do {
		arc4_s[i] = i;
		i++;
	} while(i != 0);
	do {
		j = (unsigned char )(j + arc4_s[i] + (unsigned char )(k >> i % sizeof k));
		SWAP(arc4_s[i], arc4_s[j]);
		i++;
	} while (i != 0);
}

/**
Generates a byte q (and changes the current state).

The order of operations is wrong to replicate the behavior of the executable.

@return The byte q.
**/
static unsigned char arc4(void) {
	//point A
	arc4_j = (unsigned char )(arc4_j + arc4_s[arc4_i]);
	SWAP(arc4_s[arc4_i], arc4_s[arc4_j]);
	const unsigned char q = arc4_s[(unsigned char )(arc4_s[arc4_i] + arc4_s[arc4_j])];
	arc4_i++;//should be at point A
	return q;
}

/**
Generates an integer w and increments the count c.

@return The integer w.
**/
static unsigned long int arc4_word(void) {
	arc4_c++;
	unsigned long int w = 0;
	for (size_t bit = 0; bit < sizeof w; bit++) {
		w |= (size_t )arc4() << bit * CHAR_BIT;
	}
	return w;
}

/**
Generates a bound integer b at least 0 and at most s - 1.

Emulates an internal function:
<pre>
unsigned long int (* const arc4_bound_word)(unsigned long int supremum) = (void * )0x08126130;
</pre>

@param s The supremum s.
@return The integer b.
**/
static unsigned long int arc4_bound(const unsigned long int s) {
	const unsigned long int b = arc4_word();
	if (s == 0) {
		return b;
	}
	return b % s;
}

/**
Generates and injects
 the counter c,
 the state S and
 the iterators i and j.

@param k The seed k to use.
@param calls The amount of calls to do.
**/
void arc4_inject(const unsigned long int k, const unsigned int calls) {
	arc4_i = 0;
	arc4_j = 0;
	srandom(k);
	sarc4((unsigned long int )random());
	for (unsigned int call = 0; call < exec_arc4_calls; call++) {
		arc4_word();
	}
	const unsigned long int first_sup = 20;
	const unsigned long int second_sup = 18;
	unsigned long int first = arc4_bound(first_sup) + 10;
	for (unsigned long int iterator = 0; iterator < first; iterator++) {
		const unsigned long int second = arc4_bound(second_sup);
		while (arc4_bound(second_sup) == second);
	}
	for (unsigned int call = 0; call < calls; call++) {
		arc4_word();
	}
	memcpy(exec_arc4_c, &arc4_c, sizeof arc4_c);
	memcpy(exec_arc4_s, arc4_s, sizeof arc4_s);
	memcpy(exec_arc4_i, &arc4_i, sizeof arc4_i);
	memcpy(exec_arc4_j, &arc4_j, sizeof arc4_j);
}
