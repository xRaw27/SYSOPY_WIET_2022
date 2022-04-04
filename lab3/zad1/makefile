CC=gcc
CFLAGS=-Wall
MAIN=main
RESULTS_DIR=./results
N=10
VALGRIND=

all: clean compile
	$(VALGRIND) ./$(MAIN) $(N)

compile:
	$(CC) $(CFLAGS) $(MAIN).c -o $(MAIN)

clean:
	rm -f *.o *.a *.so $(MAIN)