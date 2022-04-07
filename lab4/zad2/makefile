CC=gcc
CFLAGS=-Wall
MAIN=main
RESULTS_DIR=./results
RESULTS_PATH=./results/results.txt
VALGRIND=

all: clean compile run_tests

run_tests:
	mkdir -p $(RESULTS_DIR)
	$(VALGRIND) ./$(MAIN) > $(RESULTS_PATH)

compile:
	$(CC) $(CFLAGS) $(MAIN).c -o $(MAIN)

clean:
	rm -f *.o *.a *.so $(MAIN)