CC=gcc
CFLAGS=-Wall
MAIN=main
VALGRIND=
TEST=test.ascii.pgm
OUT=out.ascii.pgm

all: compile run_tests

compile:
	$(CC) $(CFLAGS) $(MAIN).c -o $(MAIN) -lpthread

run_tests:
	for i in 1 2 4 8 16 32 ; do \
		./$(MAIN) $$i numbers $(TEST) $(OUT) ; \
		./$(MAIN) $$i blocks $(TEST) $(OUT) ; \
	done

clean:
	rm -f *.o *.a *.so $(MAIN)