CC=g++
CFLAGS=-I -lsdc++.
DEPS =
OBJ = main.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

sig_test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)