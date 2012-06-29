RM = /bin/rm -f
MKDIR = /bin/mkdir -p
CP = /bin/cp -u
GCC = /usr/bin/gcc -std=gnu99 -ldl -lrt -lconfig -O3 -g -Wall -Wextra -Wundef -Wcast-qual -Wcast-align -Wconversion -Wshadow -Wpointer-arith -Wunreachable-code -Waggregate-return -Wswitch-default -Wswitch-enum -Wstrict-prototypes -Wstrict-overflow=5 -Wformat=2 -Wwrite-strings
#dlfcn.h requires -ldl
#sys/shm.h requires -lrt
#libconfig.h requires -lconfig
#-save-temps=obj requires -fverbose-asm
#-pedantic requires -std=c99
BIN = bin
OBJ = obj
SRC = src
NAME = adom-tas

all: $(BIN)/$(NAME).so $(BIN)/$(NAME)

clean:
	$(RM) $(OBJ)/* $(BIN)/*

prepare:
	$(MKDIR) $(OBJ)
	$(MKDIR) $(BIN)

sh:
	$(CP) $(SRC)/*.sh $(BIN)

$(OBJ)/%.o: $(SRC)/%.c
	$(GCC) -fpic -c -o $@ $<

$(BIN)/$(NAME).so: prepare $(OBJ)/launcher.o $(OBJ)/loader.o $(OBJ)/problem.o $(OBJ)/exec.o $(OBJ)/def.o $(OBJ)/fork.o $(OBJ)/record.o $(OBJ)/util.o $(OBJ)/asm.o $(OBJ)/log.o $(OBJ)/shm.o $(OBJ)/config.o $(OBJ)/lib.o $(OBJ)/put.o
	$(GCC) -fpic -shared -o $(BIN)/$(NAME).so $(OBJ)/launcher.o $(OBJ)/loader.o $(OBJ)/problem.o $(OBJ)/exec.o $(OBJ)/def.o $(OBJ)/fork.o $(OBJ)/record.o $(OBJ)/util.o $(OBJ)/asm.o $(OBJ)/log.o $(OBJ)/shm.o $(OBJ)/config.o $(OBJ)/lib.o $(OBJ)/put.o

$(BIN)/$(NAME): prepare $(OBJ)/launcher.o $(OBJ)/loader.o $(OBJ)/problem.o $(OBJ)/exec.o $(OBJ)/def.o $(OBJ)/fork.o $(OBJ)/record.o $(OBJ)/util.o $(OBJ)/asm.o $(OBJ)/log.o $(OBJ)/shm.o $(OBJ)/config.o $(OBJ)/lib.o $(OBJ)/put.o
	$(GCC) -lncurses -o $(BIN)/$(NAME) $(OBJ)/launcher.o $(OBJ)/loader.o $(OBJ)/problem.o $(OBJ)/exec.o $(OBJ)/def.o $(OBJ)/fork.o $(OBJ)/record.o $(OBJ)/util.o $(OBJ)/asm.o $(OBJ)/log.o $(OBJ)/shm.o $(OBJ)/config.o $(OBJ)/lib.o $(OBJ)/put.o

run: all
	$(BIN)/$(NAME)
