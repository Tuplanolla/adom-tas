/**
Injects not-medicine.
**/
#ifndef ASM_C
#define ASM_C

#include <string.h>//mem*
#include <sys/mman.h>//mprotect, PROT_*

#include "util.h"
#include "problem.h"//problem_t
#include "log.h"//error
#include "adom.h"
#include "asm.h"

problem_t inject_save(void (* injector)()) {
	void * position = (void * )0x08090733;
	void * location = (void * )0x0809072a;
	unsigned int f = (unsigned int )injector - (unsigned int )position;
	unsigned char original_instructions[10];
	original_instructions[0] = 0x83;//ADD
	original_instructions[1] = 0xc4;//ESP
	original_instructions[2] = 0xf8;//248
	original_instructions[3] = 0x6a;//PUSH
	original_instructions[4] = 0x00;//0
	original_instructions[5] = 0x68;//PUSH
	original_instructions[6] = 0xc4;//0x0815e4c4
	original_instructions[7] = 0xe4;
	original_instructions[8] = 0x15;
	original_instructions[9] = 0x08;
	unsigned char instructions[10];//TODO document
	instructions[0] = 0xe8;//CALL
	instructions[1] = (unsigned char )(f >> 0x00 & 0xff);//injector
	instructions[2] = (unsigned char )(f >> 0x08 & 0xff);
	instructions[3] = (unsigned char )(f >> 0x10 & 0xff);
	instructions[4] = (unsigned char )(f >> 0x18 & 0xff);
	instructions[5] = 0xe9;//JMP
	instructions[6] = 0xf3;//to RET
	instructions[7] = 0x00;
	instructions[8] = 0x00;
	instructions[9] = 0x00;
	const int prot = PROT_READ | PROT_WRITE | PROT_EXEC;
	if (mprotect(PAGE(location), PAGE_SIZE(instructions), prot) == 0) {
		memcpy(location, instructions, sizeof (instructions));//TODO make sure it's patching the right instructions
	}
	else {
		return error(MPROTECT_PROBLEM);
	}
	return NO_PROBLEM;
}

#endif
