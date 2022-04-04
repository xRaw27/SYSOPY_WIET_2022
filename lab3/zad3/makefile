CC=gcc
CFLAGS=-Wall -g
MAIN=main
TEST_PATH=/home/xraw/SYSOPY_WIET_2022
TEST_PATTERN=time
#VALGRIND=valgrind --leak-check=full -s
VALGRIND=


all: clean compile
	$(VALGRIND) ./$(MAIN) $(TEST_PATH) $(TEST_PATTERN) 4

compile:
	$(CC) $(CFLAGS) $(MAIN).c -o $(MAIN) -lm

clean:
	rm -f *.o *.a *.so *.txt $(MAIN)
