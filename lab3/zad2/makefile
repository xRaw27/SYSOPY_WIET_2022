CC=gcc
CFLAGS=-Wall
MAIN=main
RESULTS_DIR=./results
RESULTS_PATH=./results/results.txt
VALGRIND=

all: clean compile run_tests

compile:
	$(CC) $(CFLAGS) $(MAIN).c -o $(MAIN) -lm

clean:
	rm -f *.o *.a *.so w*.txt $(MAIN)

run_tests:
	mkdir -p $(RESULTS_DIR)
	echo "\nRectangle width 0.0000000123, number of child processes 1:">$(RESULTS_PATH)
	time -p $(VALGRIND) ./$(MAIN) 0.0000000123 1 >> $(RESULTS_PATH) 2>&1
	rm -f w*.txt
	echo "\nRectangle width 0.0000000123, number of child processes 10:">>$(RESULTS_PATH)
	time -p $(VALGRIND) ./$(MAIN) 0.0000000123 10 >> $(RESULTS_PATH) 2>&1
	rm -f w*.txt
	echo "\nRectangle width 0.0000000123, number of child processes 100:">>$(RESULTS_PATH)
	time -p $(VALGRIND) ./$(MAIN) 0.0000000123 100 >> $(RESULTS_PATH) 2>&1
	rm -f w*.txt
	echo "\nRectangle width 0.0000000123, number of child processes 1000:">>$(RESULTS_PATH)
	time -p $(VALGRIND) ./$(MAIN) 0.0000000123 1000 >> $(RESULTS_PATH) 2>&1
	rm -f w*.txt
	echo "\nRectangle width 0.000000001, number of child processes 1:">>$(RESULTS_PATH)
	time -p $(VALGRIND) ./$(MAIN) 0.000000001 1 >> $(RESULTS_PATH) 2>&1
	rm -f w*.txt
	echo "\nRectangle width 0.000000001, number of child processes 10:">>$(RESULTS_PATH)
	time -p $(VALGRIND) ./$(MAIN) 0.000000001 10 >> $(RESULTS_PATH) 2>&1
	rm -f w*.txt
	echo "\nRectangle width 0.000000001, number of child processes 100:">>$(RESULTS_PATH)
	time -p $(VALGRIND) ./$(MAIN) 0.000000001 100 >> $(RESULTS_PATH) 2>&1
	rm -f w*.txt
	echo "\nRectangle width 0.000000001, number of child processes 1000:">>$(RESULTS_PATH)
	time -p $(VALGRIND) ./$(MAIN) 0.000000001 1000 >> $(RESULTS_PATH) 2>&1
	rm -f w*.txt
	echo "\nRectangle width 0.00001, number of child processes 10:">>$(RESULTS_PATH)
	time -p $(VALGRIND) ./$(MAIN) 0.00001 10 >> $(RESULTS_PATH) 2>&1
	rm -f w*.txt
	echo "\nRectangle width 0.00001, number of child processes 100:">>$(RESULTS_PATH)
	time -p $(VALGRIND) ./$(MAIN) 0.00001 100 >> $(RESULTS_PATH) 2>&1
	rm -f w*.txt
	cat $(RESULTS_PATH)