CC=g++
CFLAGS=-I /usr/local/include/ -lcsv
DEPS = 
OBJ = main.o libcsv.o

%.o: %.c $(DEPS)
	$(CC) -c -g -o $@ $< $(CFLAGS)

csv_test: $(OBJ)
	$(CC) -g -o $@ $^ $(CFLAGS)