/**
Injects assembly instructions.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef ASM_C
#define ASM_C

#include <stddef.h>//size_t, ptrdiff_t
#include <string.h>//mem*
#include <sys/mman.h>//mprotect, PROT_*
#include <limits.h>//CHAR_BIT

#include "util.h"//PAGE*
#include "problem.h"//problem_t, *_PROBLEM
#include "log.h"//error, warning, note

/**
Replaces the save command with a custom function.

The original instructions:
<pre>
0x08090727:  00 00 00
0x0809072a:  83 c4 f8        add   $0xfffffff8, %esp
0x0809072d:  6a 00           push  $0x0
0x0809072f:  68 c4 e4 15 08  push  $0x0815e4c4
0x08090734:  e8 c7 b0 fb ff  call  0x0804b800
</pre>

The injected instructions:
<pre>
0x08090727:  00 00 00
0x0809072a:  e8 ?? ?? ?? ??  call  0x????????
0x0809072f:  e9 f3 00 00 00  jmp   0x08090827
0x08090734:  e8 c7 b0 fb ff  call  0x0804b800
</pre>

@param function The custom function.
@return The error code.
**/
problem_t inject_save(void (* const function)(void)) {
	const unsigned char original[10] = {
		0x83, 0xc4, 0xf8,
		0x6a, 0x00,
		0x68, 0xc4, 0xe4, 0x15, 0x08
	};
	unsigned char injected[10] = {
		0xe8, 0x00, 0x00, 0x00, 0x00,
		0xe9, 0xf3, 0x00, 0x00, 0x00
	};
	void * const location = (void * )0x0809072a;
	const ptrdiff_t offset = 0x5;
	const ptrdiff_t pointer = (ptrdiff_t )function - ((ptrdiff_t )location + offset);
	for (size_t bit = 0; bit < sizeof bit; bit++) {
		injected[bit + 1] = (unsigned char )(pointer >> (bit * CHAR_BIT) & 0xff);
	}
	const int prot = PROT_READ | PROT_WRITE | PROT_EXEC;
	if (mprotect(PAGE(location), PAGE_SIZE(original), prot) == -1) {
		return error(ASM_MPROTECT_PROBLEM);
	}
	else {
		if (memcmp(location, original, sizeof original) != 0) {
			return error(ASM_MEMCMP_PROBLEM);
		}
		else {
			memcpy(location, injected, sizeof injected);
		}
	}
	return NO_PROBLEM;
}

#endif
