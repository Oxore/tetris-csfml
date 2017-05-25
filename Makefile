CC=gcc
FILES:=$(wildcard src/*.c)
FILES:=$(FILES:.c=.o)

CFLAGS += -Wall
CFLAGS += -std=c99
CFLAGS += -O3
LIBS += -lcsfml-graphics 
LIBS += -lcsfml-window 
LIBS += -lcsfml-system 

all: tetris

tetris: $(FILES) 
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f tetris $(FILES)
