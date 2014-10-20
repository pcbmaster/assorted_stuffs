CC=g++
CFLAGS= -g
LFLAGS=-lcurl -lcsv_parser
SRC=main
TARGET=skeletor

all: $(TARGET)

$(TARGET): $(SRC).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC).cpp $(LFLAGS)