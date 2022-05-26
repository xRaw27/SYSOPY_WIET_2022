CC=gcc
CFLAGS=-Wall
MAIN=main
VALGRIND=

all: compile

compile:
	$(CC) $(CFLAGS) $(MAIN).c -o $(MAIN) -lpthread

clean:
	rm -f *.o *.a *.so $(MAIN)