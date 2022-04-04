CC=gcc
CFLAGS=-Wall
MAIN=main
RESULTS_DIR=./results
VALGRIND=

all: clean compile
	$(VALGRIND) ./$(MAIN)

print:
	echo $(VALGRIND)

compile:
	$(CC) $(CFLAGS) $(MAIN).c -o $(MAIN)

clean:
	rm -f *.o *.a *.so $(MAIN)