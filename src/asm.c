/**
Injects assembly instructions.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#include <stddef.h>//size_t, ptrdiff_t
#include <string.h>//mem*
#include <limits.h>//CHAR_BIT
#include <sys/mman.h>//mprotect, PROT_*

#include "util.h"//PAGE*
#include "prob.h"//probno, *_PROBLEM
#include "log.h"//log_*

/**
Replaces the save command with a custom function.

The call is left out if the function is <code>NULL</code>.

The original instructions:
<pre>
0x08090727:  00 00 00
0x0809072a:  83 c4 f8        add   $0xfffffff8, %esp <sub $0x7>
0x0809072d:  6a 00           push  $0x0
0x0809072f:  68 c4 e4 15 08  push  $0x0815e4c4
0x08090734:  e8 c7 b0 fb ff  call  0x0804b800
</pre>

The injected instructions:
<pre>
0x08090727:  00 00 00
0x0809072a:  e8 ?? ?? ?? ??  call  0x???????? <function>
0x0809072f:  e9 f3 00 00 00  jmp   0x08090827
0x08090734:  e8 c7 b0 fb ff  call  0x0804b800
</pre>

@param function The custom function.
@return The error code.
**/
int inject_save(void (* const function)(void)) {
	const unsigned char original[10] = {
		0x83, 0xc4, 0xf8,
		0x6a, 0x00,
		0x68, 0xc4, 0xe4, 0x15, 0x08
	};
	unsigned char injected[10] = {
		0x90, 0x90, 0x90, 0x90, 0x90,
		0xe9, 0xf3, 0x00, 0x00, 0x00
	};
	void * const location = (void * )0x0809072a;
	if (function != NULL) {
		const ptrdiff_t offset = 0x5;
		const ptrdiff_t pointer = (ptrdiff_t )function - ((ptrdiff_t )location + offset);
		injected[0] = (unsigned char )0xe8;
		for (size_t byte = 0; byte < sizeof byte; byte++) {
			injected[byte + 1] = (unsigned char )(pointer >> (byte * CHAR_BIT));
		}
	}
	void * const addr = PAGE(location);
	const size_t len = PAGE_SIZE(original);
	if (mprotect(addr, len, PROT_READ | PROT_WRITE) == -1) {//for W^X
		probno = log_error(ASM_MPROTECT_PROBLEM);
		return -1;
	}
	if (memcmp(location, original, sizeof original) != 0) {
		probno = log_error(ASM_MEMCMP_PROBLEM);
		return -1;
	}
	memcpy(location, injected, sizeof injected);
	if (mprotect(addr, len, PROT_READ | PROT_EXEC) == -1) {
		probno = log_error(ASM_MPROTECT_PROBLEM);
		return -1;
	}
	return 0;
}
