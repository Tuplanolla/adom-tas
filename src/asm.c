/**
Injects not-medicine.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef ASM_C
#define ASM_C

#include <stddef.h>//ptrdiff_t
#include <string.h>//mem*
#include <sys/mman.h>//mprotect, PROT_*

#include "util.h"//PAGE*
#include "problem.h"//problem_t, *_PROBLEM
#include "log.h"//error, warning, note

/**
Replaces the save command with a custom function.

The original instructions:
<pre>
    0x08090727:  00 00 00
=>  0x0809072a:  83 c4 f8        add   $0xfffffff8,%esp
    0x0809072d:  6a 00           push  $0x0
    0x0809072f:  68 c4 e4 15 08  push  $0x0815e4c4
    0x08090734:  e8 c7 b0 fb ff  call  0x0804b800
</pre>

The new instructions:
<pre>
    0x08090727:  00 00 00
=>  0x0809072a:  e8 ?? ?? ?? ??  call  0x????????
    0x0809072f:  e9 f3 00 00 00  jmp   0x08090827
    0x08090734:  e8 c7 b0 fb ff  call  0x0804b800
</pre>

@param function The custom function.
@return The error code.
**/
problem_t inject_save(void (* const function)(void)) {
	void * const location = (void * )0x0809072a;
	const ptrdiff_t offset = 0x09;
	const ptrdiff_t pointer = (ptrdiff_t )function - ((ptrdiff_t )location + offset);
	unsigned char old_instructions[10];
	old_instructions[0] = 0x83;
	old_instructions[1] = 0xc4;
	old_instructions[2] = 0xf8;
	old_instructions[3] = 0x6a;
	old_instructions[4] = 0x00;
	old_instructions[5] = 0x68;
	old_instructions[6] = 0xc4;
	old_instructions[7] = 0xe4;
	old_instructions[8] = 0x15;
	old_instructions[9] = 0x08;
	unsigned char new_instructions[10];
	new_instructions[0] = 0xe8;
	new_instructions[1] = (unsigned char )(pointer >> 0x00 & 0xff);//extracts the bytes
	new_instructions[2] = (unsigned char )(pointer >> 0x08 & 0xff);
	new_instructions[3] = (unsigned char )(pointer >> 0x10 & 0xff);
	new_instructions[4] = (unsigned char )(pointer >> 0x18 & 0xff);
	new_instructions[5] = 0xe9;
	new_instructions[6] = 0xf3;
	new_instructions[7] = 0x00;
	new_instructions[8] = 0x00;
	new_instructions[9] = 0x00;
	const int prot = PROT_READ | PROT_WRITE | PROT_EXEC;
	if (mprotect(PAGE(location), PAGE_SIZE(new_instructions), prot) != 0) {
		return error(ASM_MPROTECT_PROBLEM);
	}
	else {
		if (memcmp(location, old_instructions, sizeof new_instructions) != 0) {
			return error(ASM_MEMCMP_PROBLEM);
		}
		else {
			memcpy(location, new_instructions, sizeof new_instructions);
		}
	}
	return NO_PROBLEM;
}

#endif
