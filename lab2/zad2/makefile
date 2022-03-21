CC=gcc
CFLAGS=-Wall -O3
MAIN_SYS=main_sys
TEST_DIR_PATH=./test
SAMPLE=sample.txt
MULTIPLIER=60000
IN_NAME=in.txt
RESULTS_DIR=./results
RESULTS_PATH=./results/results.txt
VALGRIND=
CHAR=f

all: clean generate_test_file compile run_tests

clean:
	rm -f *.o *.a *.so main

all_valgrind:
	make all VALGRIND=valgrind -s

compile:
	$(CC) $(CFLAGS) $(MAIN_SYS).c -o $(MAIN_SYS)

generate_test_file:
	./duplicate.sh $(TEST_DIR_PATH)/$(SAMPLE) $(TEST_DIR_PATH)/$(IN_NAME) $(MULTIPLIER)

run_tests:
	mkdir -p $(RESULTS_DIR)
	echo "\nTEST FILE SIZE: ">$(RESULTS_PATH)
	ls -sh $(TEST_DIR_PATH)/$(IN_NAME)>>$(RESULTS_PATH)
	echo "\nResult times of read()/write() program with 10KB buffer:">>$(RESULTS_PATH)
	time -p $(VALGRIND) ./$(MAIN_SYS) $(TEST_DIR_PATH)/$(IN_NAME) $(CHAR) 2>>$(RESULTS_PATH)
	cat $(RESULTS_PATH)