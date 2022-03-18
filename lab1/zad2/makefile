CC=gcc
CFLAGS=-Wall
LIBRARY_NAME=lib
TEST_DIR_PATH=./test
RESULTS_DIR=./results
RESULTS_PATH=./results/results.txt
SM=./test/small
MD=./test/medium
LG=./test/large
VALGRIND=

compile:
	$(CC) -c $(CFLAGS) $(LIBRARY_NAME).c
	ar cr lib$(LIBRARY_NAME).a $(LIBRARY_NAME).o
	$(CC) $(CFLAGS) main.c -o main -L . -l $(LIBRARY_NAME)

all: clean create_random_test_files compile run_tests

run_tests: clean_results wc_files_test save_and_remove_blocks_test
	cat $(RESULTS_PATH)

run_valgrind_tests: set_valgrind clean_results wc_files_test save_and_remove_blocks_test
	cat $(RESULTS_PATH)

clean:
	rm -f *o *a main temp
	rm -rf $(TEST_DIR_PATH)

clean_results:
	mkdir -p $(RESULTS_DIR)
	rm -f $(RESULTS_PATH)

set_valgrind:
	$(eval VALGRIND := valgrind -s)

create_random_test_files:
	rm -rf $(TEST_DIR_PATH)
	mkdir $(TEST_DIR_PATH)
	for number in 0 1 2 3 4 5 6 7 8 9 ; do \
  		dd if=/dev/urandom iflag=fullblock of=$(TEST_DIR_PATH)/small_$$number bs=1M count=1 ; \
  		dd if=/dev/urandom iflag=fullblock of=$(TEST_DIR_PATH)/medium_$$number bs=16M count=1 ; \
  		dd if=/dev/urandom iflag=fullblock of=$(TEST_DIR_PATH)/large_$$number bs=32M count=4 ; \
	done

wc_files_test:
	$(VALGRIND) ./main -s $(RESULTS_PATH) -c 10 -t -w $(SM)_0 -e "wc 1 small (1MB) file" \
							  -t -w $(SM)_0,$(SM)_1,$(SM)_2,$(SM)_3,$(SM)_4 -e "wc 5 small (1MB) files" \
							  -t -w $(SM)_0,$(SM)_1,$(SM)_2,$(SM)_3,$(SM)_4,$(SM)_5,$(SM)_6,$(SM)_7,$(SM)_8,$(SM)_9 -e "wc 10 small (1MB) files" \
							  -t -w $(MD)_0 -e "wc 1 medium (16MB) file" \
							  -t -w $(MD)_0,$(MD)_1,$(MD)_2,$(MD)_3,$(MD)_4 -e "wc 5 medium (16MB) files" \
							  -t -w $(MD)_0,$(MD)_1,$(MD)_2,$(MD)_3,$(MD)_4,$(MD)_5,$(MD)_6,$(MD)_7,$(MD)_8,$(MD)_9 -e "wc 10 medium (16MB) files" \
							  -t -w $(LG)_0 -e "wc 1 large (128MB) file" \
							  -t -w $(LG)_0,$(LG)_1,$(LG)_2,$(LG)_3,$(LG)_4 -e "wc 5 large (128MB) files" \
							  -t -w $(LG)_0,$(LG)_1,$(LG)_2,$(LG)_3,$(LG)_4,$(LG)_5,$(LG)_6,$(LG)_7,$(LG)_8,$(LG)_9 -e "wc 10 large (128MB) files"

save_and_remove_blocks_test:
	$(VALGRIND) ./main -s $(RESULTS_PATH) -c 40000 -w $(SM)_0,$(SM)_1,$(SM)_2 \
							  -t -b -e "save 1 block with result of wc 3 files" \
							  -t -n 1024 -e "save 1024 blocks with result of wc 3 files" \
							  -t -n 16384 -e "save 16384 blocks with result of wc 3 files" \
							  -w $(SM)_0,$(SM)_1,$(SM)_2,$(SM)_3,$(SM)_4,$(SM)_5,$(SM)_6,$(SM)_7,$(SM)_8,$(SM)_9,$(SM)_0,$(SM)_1,$(SM)_2,$(SM)_3,$(SM)_4,$(SM)_5,$(SM)_6,$(SM)_7,$(SM)_8,$(SM)_9,$(SM)_0,$(SM)_1,$(SM)_2,$(SM)_3,$(SM)_4,$(SM)_5,$(SM)_6,$(SM)_7,$(SM)_8,$(SM)_9,$(SM)_0,$(SM)_1,$(SM)_2,$(SM)_3,$(SM)_4,$(SM)_5,$(SM)_6,$(SM)_7,$(SM)_8,$(SM)_9,$(SM)_0,$(SM)_1,$(SM)_2,$(SM)_3,$(SM)_4,$(SM)_5,$(SM)_6,$(SM)_7,$(SM)_8,$(SM)_9,$(SM)_0,$(SM)_1,$(SM)_2,$(SM)_3,$(SM)_4,$(SM)_5,$(SM)_6,$(SM)_7,$(SM)_8,$(SM)_9,$(SM)_0,$(SM)_1,$(SM)_2,$(SM)_3,$(SM)_4,$(SM)_5,$(SM)_6,$(SM)_7,$(SM)_8,$(SM)_9,$(SM)_0,$(SM)_1,$(SM)_2,$(SM)_3,$(SM)_4,$(SM)_5,$(SM)_6,$(SM)_7,$(SM)_8,$(SM)_9,$(SM)_0,$(SM)_1,$(SM)_2,$(SM)_3,$(SM)_4,$(SM)_5,$(SM)_6,$(SM)_7,$(SM)_8,$(SM)_9,$(SM)_0,$(SM)_1,$(SM)_2,$(SM)_3,$(SM)_4,$(SM)_5,$(SM)_6,$(SM)_7,$(SM)_8,$(SM)_9 \
							  -t -b -e "save 1 block with result of wc 100 files" \
							  -t -n 1024 -e "save 1024 blocks with result of wc 100 files" \
							  -t -n 16384 -e "save 16384 blocks with result of wc 100 files" \
							  -t -r 0 -e "remove 1 block with result of wc 3 files" \
							  -t -m "1024 1" -e "remove 1024 blocks with result of wc 3 files" \
							  -t -m "16384 1025" -e "remove 16384 blocks with result of wc 3 files" \
							  -t -r "17409" "remove 1 block with result of wc 100 files" \
							  -t -m "1024 17410" -e "remove 1024 blocks with result of wc 100 files" \
							  -t -m "16384 18434" -e "remove 16384 blocks with result of wc 100 files"
	$(VALGRIND) ./main -s $(RESULTS_PATH) -c 16384 -w $(SM)_0,$(SM)_1,$(SM)_2 \
							  -t -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0"\
							  -e "save and remove 16384 blocks with result of wc 3 files, 10 times" \
							  -w $(SM)_0,$(SM)_1,$(SM)_2,$(SM)_3,$(SM)_4,$(SM)_5,$(SM)_6,$(SM)_7,$(SM)_8,$(SM)_9,$(SM)_0,$(SM)_1,$(SM)_2,$(SM)_3,$(SM)_4,$(SM)_5,$(SM)_6,$(SM)_7,$(SM)_8,$(SM)_9,$(SM)_0,$(SM)_1,$(SM)_2,$(SM)_3,$(SM)_4,$(SM)_5,$(SM)_6,$(SM)_7,$(SM)_8,$(SM)_9,$(SM)_0,$(SM)_1,$(SM)_2,$(SM)_3,$(SM)_4,$(SM)_5,$(SM)_6,$(SM)_7,$(SM)_8,$(SM)_9,$(SM)_0,$(SM)_1,$(SM)_2,$(SM)_3,$(SM)_4,$(SM)_5,$(SM)_6,$(SM)_7,$(SM)_8,$(SM)_9,$(SM)_0,$(SM)_1,$(SM)_2,$(SM)_3,$(SM)_4,$(SM)_5,$(SM)_6,$(SM)_7,$(SM)_8,$(SM)_9,$(SM)_0,$(SM)_1,$(SM)_2,$(SM)_3,$(SM)_4,$(SM)_5,$(SM)_6,$(SM)_7,$(SM)_8,$(SM)_9,$(SM)_0,$(SM)_1,$(SM)_2,$(SM)_3,$(SM)_4,$(SM)_5,$(SM)_6,$(SM)_7,$(SM)_8,$(SM)_9,$(SM)_0,$(SM)_1,$(SM)_2,$(SM)_3,$(SM)_4,$(SM)_5,$(SM)_6,$(SM)_7,$(SM)_8,$(SM)_9,$(SM)_0,$(SM)_1,$(SM)_2,$(SM)_3,$(SM)_4,$(SM)_5,$(SM)_6,$(SM)_7,$(SM)_8,$(SM)_9 \
							  -t -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" -n 16384 -m "16384 0" \
							  -e "save and remove 16384 blocks with result of wc 100 files, 30 times"
	dd if=/dev/urandom iflag=fullblock of=temp bs=32M count=4
	$(VALGRIND) ./main -s $(RESULTS_PATH) -t -c 1 -b -r 0 -e "save and remove random large (128MB) temp file"
