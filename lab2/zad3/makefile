CC=gcc
CFLAGS=-Wall
MAIN_STAT=main_stat
MAIN_NFTW=main_nftw
RESULTS_DIR=./results
RESULTS_STAT=./results/stat.txt
RESULTS_NFTW=./results/nftw.txt
TEST_PATH=/home/xraw/Desktop/sysopy
VALGRIND=


all: clean compile
	mkdir -p $(RESULTS_DIR)
	./$(MAIN_STAT) $(TEST_PATH) > $(RESULTS_STAT)
	tail -n 64 $(RESULTS_STAT)
	./$(MAIN_NFTW) $(TEST_PATH) > $(RESULTS_NFTW)
	tail -n 64 $(RESULTS_NFTW)

compile:
	$(CC) $(CFLAGS) $(MAIN_STAT).c -o $(MAIN_STAT)
	$(CC) $(CFLAGS) $(MAIN_NFTW).c -o $(MAIN_NFTW)

clean:
	rm -f *.o *.a *.so $(MAIN_STAT) $(MAIN_NFTW)
	rm -r $(RESULTS_DIR)