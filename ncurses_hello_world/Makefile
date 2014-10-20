CC=gcc
CFLAGS=-I
LIBS=-lncurses
OUTFILE=hello_world
DEPS=

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

make: main.o
	$(CC) -o $(OUTFILE) main.o