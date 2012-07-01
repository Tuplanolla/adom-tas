/**
Generates code automatically.

@author Sampsa "Tuplanolla" Kiiskinen
**/
#ifndef META_C
#define META_C

#include <stdlib.h>//EXIT_*
#include <stdio.h>//*print*
#include <string.h>//str*

#include <curses.h>//KEY_*

/**
Generates the <code>key_code</code> function.
**/
void key_code_meta(void) {
	printf("const char * key_code(const int key) {\n"
			"\tswitch (key) {\n");
	int key = 0;
	do {
		char code[7];
		#define SNPRINTF(...) snprintf(code, sizeof code, __VA_ARGS__)
		if (key == '\\') SNPRINTF("\\\\");//backslash
		else if (key == ' ') SNPRINTF("\\S");//Space
		else if (key == 0x1b) SNPRINTF("\\M");//Meta (Alt or Esc)
		else if (key == 0x7f) SNPRINTF("\\C_");//Delete
		else if (key == KEY_UP) SNPRINTF("\\U");//Up
		else if (key == KEY_DOWN) SNPRINTF("\\D");//Down
		else if (key == KEY_LEFT) SNPRINTF("\\L");//Left
		else if (key == KEY_RIGHT) SNPRINTF("\\R");//Right
		else if (key == KEY_A1) SNPRINTF("\\H");//keypad Home
		else if (key == KEY_A3) SNPRINTF("\\+");//keypad PageUp
		else if (key == KEY_B2) SNPRINTF("\\.");//keypad center
		else if (key == KEY_C1) SNPRINTF("\\E");//keypad End
		else if (key == KEY_C3) SNPRINTF("\\-");//keypad PageDown
		else if (key >= 0x00 && key < 0x1f) SNPRINTF("\\C%c", (char )(0x60 + key));//control keys
		else if (key >= KEY_F(1) && key <= KEY_F(64)) SNPRINTF("\\%u", (char )(key - KEY_F(0)));//function keys
		else if (key > 0x20 && key < 0x80) SNPRINTF("%c", (char )key);//printable keys
		else goto end;
		unsigned int first = 0;
		while (first < sizeof code - 1) {
			if (code[first] == '\\' || code[first] == '"') {
				unsigned int last = sizeof code - 1;
				while (last > first) {
					code[last] = code[last - 1];
					last--;
				}
				code[first] = '\\';
				first++;
			}
			first++;
		}
		printf("\t\tcase %d: return \"%s\";\n", key, code);
		end: key++;
	} while (key <= 0xffff);
	printf("\t\tdefault: return \"\\\\?\";\n"
			"\t}\n"
			"}\n");
}

/**
Generates code.

@param argc The amount of functions to generate.
@param argv The names of functions to generate.
@return <code>EXIT_SUCCESS</code>.
**/
int main(int argc, const char * const argv[]) {
	while (argc > 1) {
		argc--;
		if (strcmp(argv[argc], "key_code") == 0) {
			key_code_meta();
		}
	}

	return EXIT_SUCCESS;
}

#endif
