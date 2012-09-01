RM = rm -f
MKDIR = mkdir -p
CP = cp -u
CC = gcc
SIZE = 32767
CFLAGS = -std=gnu99 -O3\
		-ldl -lrt -lconfig -lncurses\
		-g -save-temps=obj -fverbose-asm\
		-Wall -Wextra
#		-Waddress -Waggregate-return -Wall -Warray-bounds -Wcast-align -Wcast-qual -Wchar-subscripts -Wclobbered -Wcomment -Wconversion -Wcoverage-mismatch -Wdisabled-optimization -Wempty-body -Wenum-compare -Wextra -Wfloat-equal -Wformat -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wformat=2 -Wframe-larger-than=$(SIZE) -Wignored-qualifiers -Wimplicit -Wimplicit-function-declaration -Wimplicit-int -Winit-self -Winline -Winvalid-pch -Wlarger-than=$(SIZE) -Wlogical-op -Wlong-long -Wmain -Wmissing-braces -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wno-attributes -Wno-builtin-macro-redefined -Wno-deprecated -Wno-deprecated-declarations -Wno-div-by-zero -Wno-endif-labels -Wno-format-contains-nul -Wno-format-extra-args -Wno-int-to-pointer-cast -Wno-mudflap -Wno-multichar -Wno-overflow -Wno-pointer-to-int-cast -Wno-pragmas -Wnonnull -Woverlength-strings -Wpacked -Wpacked-bitfield-compat -Wpadded -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point -Wshadow -Wsign-compare -Wsign-conversion -Wstack-protector -Wstrict-aliasing -Wstrict-overflow=5 -Wswitch -Wswitch-default -Wswitch-enum -Wsync-nand -Wsystem-headers -Wtrigraphs -Wtype-limits -Wundef -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wunsafe-loop-optimizations -Wunused -Wunused-function -Wunused-label -Wunused-parameter -Wunused-value -Wunused-variable -Wvariadic-macros -Wvla -Wvolatile-register-var -Wwrite-strings
#-std=gnu99 for *env, sig*, shm* and *random
#-ldl for dlfcn.h
#-lrt for sys/shm.h
#-lncurses for curses.h
#-lconfig for libconfig.h
#-fverbose-asm for -save-temps=obj
BIN = bin
OBJ = obj
SRC = src
META = src/meta
NAME = adom-tas
MAIN = $(BIN)/$(NAME)
LIB = $(BIN)/$(NAME).so
MAIN_DEP = $(OBJ)/main.o $(OBJ)/cfg.o $(OBJ)/def.o $(OBJ)/exec.o $(OBJ)/log.o $(OBJ)/prob.o $(OBJ)/proj.o $(OBJ)/arc4.o $(OBJ)/util.o $(OBJ)/rec.o
LIB_DEP = $(OBJ)/asm.o $(OBJ)/exec.o $(OBJ)/gui.o $(OBJ)/put.o $(OBJ)/shm.o $(OBJ)/cfg.o $(OBJ)/lib.o $(OBJ)/meta.o $(OBJ)/rec.o $(OBJ)/def.o $(OBJ)/log.o $(OBJ)/prob.o $(OBJ)/util.o $(OBJ)/proj.o $(OBJ)/arc4.o# $(OBJ)/play.o# $(OBJ)/roll.o

all: $(LIB) $(MAIN) sh
	echo -e "\n\n--- The project is still unfinished; running the executable is discouraged. ---\n\n"

run: $(LIB) $(MAIN)
	$(MAIN)

clean:
	$(RM) $(META)/* $(OBJ)/* $(BIN)/*

dirs:
	$(MKDIR) $(META)
	$(MKDIR) $(OBJ)
	$(MKDIR) $(BIN)

sh:
	$(CP) $(SRC)/*.sh $(BIN)

meta:
	$(CC) $(CFLAGS) $(SRC)/meta.c -o $(OBJ)/meta
	$(OBJ)/meta key_code >$(META)/key_code.c

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -fpic -c -o $@ $<
	$(CC) $(CFLAGS) -MM -fpic -c $(SRC)/$*.c > $(OBJ)/$*.d
	
-include $(MAIN_DEP:.o=.d)
-include $(LIB_DEP:.o=.d)

$(LIB): dirs meta $(LIB_DEP)
	$(CC) $(CFLAGS) -fpic -shared -o $(LIB) $(LIB_DEP)

$(MAIN): dirs $(MAIN_DEP) $(LIB)
	$(CC) $(CFLAGS) -o $(MAIN) $(MAIN_DEP)
