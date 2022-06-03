CC=gcc
CFLAGS=-Wall
MAIN=main
VALGRIND=

all: compile
	./$(MAIN)

compile:
	$(CC) $(CFLAGS) $(MAIN).c -o $(MAIN) -lpthread

clean:
	rm -f *.o *.a *.so $(MAIN)
