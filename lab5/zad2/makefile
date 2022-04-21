CC=gcc
CFLAGS=-Wall
MAIN=main
VALGRIND=

all: clean compile

compile:
	$(CC) $(CFLAGS) $(MAIN).c -o $(MAIN)

clean:
	rm -f *.o *.a *.so $(MAIN)