RM = /bin/rm -f
MKDIR = /bin/mkdir -p
GCC = /usr/bin/gcc -std=gnu99 -ldl -lrt -O3 -Wall
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

$(OBJ)/%.o: $(SRC)/%.c
	$(GCC) -fpic -c -o $@ $<

$(BIN)/$(NAME).so: prepare $(OBJ)/loader.o
	$(GCC) -fpic -shared -o $(BIN)/$(NAME).so $(OBJ)/loader.o

$(BIN)/$(NAME): prepare $(OBJ)/wrapper.o
	$(GCC) -lncurses -o $(BIN)/$(NAME) $(OBJ)/wrapper.o

run: all
	$(BIN)/$(NAME)
