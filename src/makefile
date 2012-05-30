COMPILER = /usr/bin/gcc
FLAGS = -std=c99 -w -O3 -ldl -lrt# -Wall

SOURCE = adom-tas.c main.c
OBJECT = adom-tas.o main.o
SHARED = adom-tas.so
BINARY = adom-tas

all: $(SHARED) $(BINARY)

clean:
	/bin/rm -f $(OBJECT) $(SHARED) $(BINARY)

$(BINARY): $(SOURCE)
	$(COMPILER) -lncurses $(FLAGS) -o $(BINARY) $(OBJECT)

$(SHARED): $(OBJECT)
	$(COMPILER) -fpic -shared $(FLAGS) -o $(SHARED) $(OBJECT)

#$(OBJECT): $(SOURCE)
%.o: %.c
	$(COMPILER) -fpic -c $(FLAGS) -o $@ $<
