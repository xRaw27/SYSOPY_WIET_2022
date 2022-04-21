CC=gcc
CFLAGS=-Wall
MAIN=main
VALGRIND=

all: clean compile run_tests

run_tests:
	$(VALGRIND) ./$(MAIN) commands

compile:
	$(CC) $(CFLAGS) $(MAIN).c -o $(MAIN)

clean:
	rm -f *.o *.a *.so $(MAIN)