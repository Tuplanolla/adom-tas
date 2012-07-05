RM = /bin/rm -f
MKDIR = /bin/mkdir -p
CP = /bin/cp -u
GCC = /usr/bin/gcc -std=gnu99 -O3\
		-ldl -lrt -lconfig -lncurses\
		-g -save-temps=obj -fverbose-asm\
		-Waddress -Waggregate-return -Wall -Warray-bounds -Wcast-align -Wcast-qual -Wchar-subscripts -Wclobbered -Wcomment -Wconversion -Wcoverage-mismatch -Wdisabled-optimization -Wempty-body -Wenum-compare -Wextra -Wfloat-equal -Wformat -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wformat=2 -Wframe-larger-than=1024 -Wignored-qualifiers -Wimplicit -Wimplicit-function-declaration -Wimplicit-int -Winit-self -Winline -Winvalid-pch -Wlarger-than=1024 -Wlogical-op -Wlong-long -Wmain -Wmissing-braces -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wno-attributes -Wno-builtin-macro-redefined -Wno-deprecated -Wno-deprecated-declarations -Wno-div-by-zero -Wno-endif-labels -Wno-format-contains-nul -Wno-format-extra-args -Wno-int-to-pointer-cast -Wno-mudflap -Wno-multichar -Wno-overflow -Wno-pointer-to-int-cast -Wno-pragmas -Wnonnull -Woverlength-strings -Wpacked -Wpacked-bitfield-compat -Wpadded -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point -Wshadow -Wsign-compare -Wsign-conversion -Wstack-protector -Wstrict-aliasing -Wstrict-overflow=5 -Wswitch -Wswitch-default -Wswitch-enum -Wsync-nand -Wsystem-headers -Wtrigraphs -Wtype-limits -Wundef -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wunsafe-loop-optimizations -Wunused -Wunused-function -Wunused-label -Wunused-parameter -Wunused-value -Wunused-variable -Wvariadic-macros -Wvla -Wvolatile-register-var -Wwrite-strings
#		-Wall -Wextra
#-std=gnu99 for *env, sig*, shm* and *random
#-ldl for dlfcn.h
#-lrt for sys/shm.h
#-lncurses for curses.h
#-lconfig for libconfig.h
#-fverbose-asm for -save-temps=obj
BIN = bin
OBJ = obj
SRC = src
NAME = adom-tas

all: $(BIN)/$(NAME).so $(BIN)/$(NAME)

clean:
	$(RM) $(SRC)/meta/* $(OBJ)/* $(BIN)/*

prepare:
	$(MKDIR) $(SRC)/meta
	$(MKDIR) $(OBJ)
	$(MKDIR) $(BIN)

sh:
	$(CP) $(SRC)/*.sh $(BIN)

meta:
	$(GCC) $(SRC)/meta.c -o $(OBJ)/meta
	$(OBJ)/meta key_code > $(SRC)/meta/key_code.c

$(OBJ)/%.o: $(SRC)/%.c
	$(GCC) -fpic -c -o $@ $<

$(BIN)/$(NAME).so: prepare meta $(OBJ)/main.o $(OBJ)/loader.o $(OBJ)/problem.o $(OBJ)/exec.o $(OBJ)/def.o $(OBJ)/fork.o $(OBJ)/record.o $(OBJ)/util.o $(OBJ)/asm.o $(OBJ)/log.o $(OBJ)/shm.o $(OBJ)/config.o $(OBJ)/lib.o $(OBJ)/put.o
	$(GCC) -fpic -shared -o $(BIN)/$(NAME).so $(OBJ)/main.o $(OBJ)/loader.o $(OBJ)/problem.o $(OBJ)/exec.o $(OBJ)/def.o $(OBJ)/fork.o $(OBJ)/record.o $(OBJ)/util.o $(OBJ)/asm.o $(OBJ)/log.o $(OBJ)/shm.o $(OBJ)/config.o $(OBJ)/lib.o $(OBJ)/put.o

$(BIN)/$(NAME): prepare meta $(OBJ)/main.o $(OBJ)/loader.o $(OBJ)/problem.o $(OBJ)/exec.o $(OBJ)/def.o $(OBJ)/fork.o $(OBJ)/record.o $(OBJ)/util.o $(OBJ)/asm.o $(OBJ)/log.o $(OBJ)/shm.o $(OBJ)/config.o $(OBJ)/lib.o $(OBJ)/put.o
	$(GCC) -o $(BIN)/$(NAME) $(OBJ)/main.o $(OBJ)/loader.o $(OBJ)/problem.o $(OBJ)/exec.o $(OBJ)/def.o $(OBJ)/fork.o $(OBJ)/record.o $(OBJ)/util.o $(OBJ)/asm.o $(OBJ)/log.o $(OBJ)/shm.o $(OBJ)/config.o $(OBJ)/lib.o $(OBJ)/put.o

run: all
	$(BIN)/$(NAME)
