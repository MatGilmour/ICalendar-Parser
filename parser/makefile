CC = gcc
CFLAGS = -g -Wall -std=c11 

BIN = bin/
SRC = src/

all: list parser

LinkedListAPI.o: $(SRC)LinkedListAPI.c
	$(CC) $(CFLAGS) -fpic -Iinclude -c $(SRC)LinkedListAPI.c -o $(BIN)LinkedListAPI.o

CalendarParser.o: $(SRC)CalendarParser.c 
	$(CC) $(CFLAGS) -fpic -Iinclude -c $(SRC)CalendarParser.c -o $(BIN)CalendarParser.o

parserHelper.o: $(SRC)parserHelper.c 
	$(CC) $(CFLAGS) -fpic -Iinclude -c $(SRC)parserHelper.c -o $(BIN)parserHelper.o

list: LinkedListAPI.o
	gcc -shared -o ./bin/liblist.so $(BIN)LinkedListAPI.o

parser: CalendarParser.o parserHelper.o LinkedListAPI.o
	gcc -shared -o ../sharedLib.so $(BIN)parserHelper.o $(BIN)CalendarParser.o $(BIN)LinkedListAPI.o

clean:
	rm ./bin/*
