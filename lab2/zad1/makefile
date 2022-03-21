CC=gcc
CFLAGS=-Wall -O3
MAIN_SYS=main_sys
MAIN_LIB=main_lib
TEST_DIR_PATH=./test
SAMPLE=sample.txt
MULTIPLIER=60000
IN_NAME=in.txt
OUT_NAME=out.txt
RESULTS_DIR=./results
RESULTS_PATH=./results/results.txt
VALGRIND=

all: clean generate_test_file compile run_tests

clean:
	rm -f *.o *.a *.so $(MAIN_SYS) $(MAIN_LIB)
	find $(TEST_DIR_PATH) ! -name $(SAMPLE) -type f -delete

all_valgrind:
	make all VALGRIND="valgrind -s"

compile:
	$(CC) $(CFLAGS) $(MAIN_SYS).c -o $(MAIN_SYS)
	$(CC) $(CFLAGS) $(MAIN_LIB).c -o $(MAIN_LIB)

generate_test_file:
	./duplicate.sh $(TEST_DIR_PATH)/$(SAMPLE) $(TEST_DIR_PATH)/$(IN_NAME) $(MULTIPLIER)

run_tests:
	mkdir -p $(RESULTS_DIR)
	echo "\nTEST FILE SIZE: ">$(RESULTS_PATH)
	ls -sh $(TEST_DIR_PATH)/$(IN_NAME)>>$(RESULTS_PATH)
	echo "\nResult times of read()/write() program with 1KB buffer:">>$(RESULTS_PATH)
	time -p $(VALGRIND) ./$(MAIN_SYS) -i $(TEST_DIR_PATH)/$(IN_NAME) -o $(TEST_DIR_PATH)/$(OUT_NAME) -b 1024 2>>$(RESULTS_PATH)
	echo "\nResult times of read()/write() program with 10KB buffer:">>$(RESULTS_PATH)
	time -p $(VALGRIND) ./$(MAIN_SYS) -i $(TEST_DIR_PATH)/$(IN_NAME) -o $(TEST_DIR_PATH)/$(OUT_NAME) -b 10240 2>>$(RESULTS_PATH)
	echo "\nResult times of read()/write() program with 100KB buffer:">>$(RESULTS_PATH)
	time -p $(VALGRIND) ./$(MAIN_SYS) -i $(TEST_DIR_PATH)/$(IN_NAME) -o $(TEST_DIR_PATH)/$(OUT_NAME) -b 102400 2>>$(RESULTS_PATH)
	echo "\nResult times of fread()/fwrite() program:">>$(RESULTS_PATH)
	time -p $(VALGRIND) ./$(MAIN_LIB) -i $(TEST_DIR_PATH)/$(IN_NAME) -o $(TEST_DIR_PATH)/$(OUT_NAME) 2>>$(RESULTS_PATH)
	cat $(RESULTS_PATH)

