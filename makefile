RM = /bin/rm -f
MKDIR = /bin/mkdir -p
CP = /bin/cp -u
GCC = /usr/bin/gcc -std=gnu99 -ldl -lrt -lconfig -O3 -Wall -g
BIN = bin
OBJ = obj
SRC = src
NAME = adom-tas

nothing:
	echo "Don't bother trying."

all: $(BIN)/$(NAME).so $(BIN)/$(NAME) sh

clean:
	$(RM) $(OBJ)/* $(BIN)/*

prepare:
	$(MKDIR) $(OBJ)
	$(MKDIR) $(BIN)

sh:
	$(CP) $(SRC)/*.sh $(BIN)

$(OBJ)/%.o: $(SRC)/%.c
	$(GCC) -fpic -c -o $@ $<

$(BIN)/$(NAME).so: prepare $(OBJ)/loader.o $(OBJ)/log.o $(OBJ)/adom.o $(OBJ)/record.o $(OBJ)/put.o
	$(GCC) -fpic -shared -o $(BIN)/$(NAME).so $(OBJ)/loader.o $(OBJ)/log.o $(OBJ)/adom.o $(OBJ)/record.o $(OBJ)/put.o

$(BIN)/$(NAME): prepare $(OBJ)/launcher.o $(OBJ)/log.o $(OBJ)/adom.o
	$(GCC) -lncurses -o $(BIN)/$(NAME) $(OBJ)/launcher.o $(OBJ)/log.o $(OBJ)/adom.o

run: all
	$(BIN)/$(NAME)
