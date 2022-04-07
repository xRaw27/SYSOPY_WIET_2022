CC=gcc
CFLAGS=-Wall
MAIN=main
MAIN_EXEC=exec_test
RESULTS_DIR=./results
RESULTS_PATH=./results/results.txt
VALGRIND=

all: clean compile run_tests

run_tests:
	mkdir -p $(RESULTS_DIR)

	echo "\nIGNORE - FORK" > $(RESULTS_PATH)
	$(VALGRIND) ./$(MAIN) ignore fork >> $(RESULTS_PATH) 2>&1; true
	echo "\nIGNORE - EXEC" >> $(RESULTS_PATH)
	$(VALGRIND) ./$(MAIN) ignore exec >> $(RESULTS_PATH) 2>&1; true

	echo "\nHANDLER - FORK" >> $(RESULTS_PATH)
	$(VALGRIND) ./$(MAIN) handler fork >> $(RESULTS_PATH) 2>&1; true
	echo "\nHANDLER - EXEC" >> $(RESULTS_PATH)
	$(VALGRIND) ./$(MAIN) handler exec >> $(RESULTS_PATH) 2>&1; true

	echo "\nMASK - FORK" >> $(RESULTS_PATH)
	$(VALGRIND) ./$(MAIN) mask fork >> $(RESULTS_PATH) 2>&1; true
	echo "\nMASK - EXEC" >> $(RESULTS_PATH)
	$(VALGRIND) ./$(MAIN) mask exec >> $(RESULTS_PATH) 2>&1; true

	echo "\nPENDING - FORK" >> $(RESULTS_PATH)
	$(VALGRIND) ./$(MAIN) pending fork >> $(RESULTS_PATH) 2>&1; true
	echo "\nPENDING - EXEC" >> $(RESULTS_PATH)
	$(VALGRIND) ./$(MAIN) pending exec >> $(RESULTS_PATH) 2>&1; true

compile:
	$(CC) $(CFLAGS) $(MAIN).c -o $(MAIN)
	$(CC) $(CFLAGS) $(MAIN_EXEC).c -o $(MAIN_EXEC)

clean:
	rm -f *.o *.a *.so $(MAIN) $(MAIN_EXEC)