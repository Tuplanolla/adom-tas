COMPILER = /usr/bin/gcc
FLAGS = -std=c99 -w -O3 -ldl #-Wall

SOURCE = adom-tas.c
OBJECT = adom-tas.o
SHARED = adom-tas.so
BINARY = adom-tas

all: $(SHARED) $(BINARY)

clean:
	/bin/rm -f $(OBJECT) $(SHARED) $(BINARY)

$(BINARY): $(SOURCE)
	$(COMPILER) -lncurses $(FLAGS) -o $(BINARY) $(OBJECT)

$(SHARED): $(OBJECT)
	$(COMPILER) -fpic -shared $(FLAGS) -o $(SHARED) $(OBJECT)

#%.o: %.c
$(OBJECT): $(SOURCE)
	$(COMPILER) -fpic -c $(FLAGS) -o $@ $<
