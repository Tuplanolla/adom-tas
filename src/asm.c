/**
Injects not-medicine.
**/
#ifndef ASM_C
#define ASM_C

#include <stdlib.h>//TODO get rid of the unnecessary
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include <curses.h>
#include <libconfig.h>

#include "util.h"
#include "adom.h"
#include "problem.h"
#include "asm.h"

problem_t inject_save(void (* injector)()) {
	void * position = (void * )0x08090733;
	void * location = (void * )0x0809072a;
	unsigned int f = (unsigned int )injector - (unsigned int )position;
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
	if (mprotect(PAGE(location), PAGE_SIZE(instructions), PROT_READ | PROT_WRITE | PROT_EXEC) == 0) {
		memcpy(location, instructions, sizeof (instructions));//TODO make sure it's patching the right instructions
	}
	else {
		uninit(error(NO_PROBLEM));//INJECTION_PROBLEM
	}
	return NO_PROBLEM;
}

#endif
