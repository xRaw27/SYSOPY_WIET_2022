CC=gcc
CFLAGS=-Wall
COOK=cook
DELIVERER=deliverer
MAIN=main
VALGRIND=

all: compile

compile:
	$(CC) $(CFLAGS) $(COOK).c -o $(COOK)
	$(CC) $(CFLAGS) $(DELIVERER).c -o $(DELIVERER)
	$(CC) $(CFLAGS) $(MAIN).c -o $(MAIN)

clean:
	rm -f *.o *.a *.so $(COOK) $(DELIVERER) $(MAIN)