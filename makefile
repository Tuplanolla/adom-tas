RM = /bin/rm -f
MKDIR = /bin/mkdir -p
GCC = /usr/bin/gcc
FLAGS = -std=c99 -ldl -lrt -O3 -w# -Wall
SOURCE = src/adom-tas.c src/main.c
OBJECT = obj/adom-tas.o obj/main.o
SHARED = bin/adom-tas.so
BINARY = bin/adom-tas

all: $(SHARED) $(BINARY)

clean:
	$(RM) $(OBJECT) $(SHARED) $(BINARY)

$(BINARY): $(SOURCE)
	$(MKDIR) bin
	$(GCC) -lncurses $(FLAGS) -o $(BINARY) $(OBJECT)

$(SHARED): $(OBJECT)
	$(MKDIR) bin
	$(GCC) -fpic -shared $(FLAGS) -o $(SHARED) $(OBJECT)

#$(OBJECT): $(SOURCE)
obj/%.o: src/%.c
	$(MKDIR) obj
	$(GCC) -fpic -c $(FLAGS) -o $@ $<
