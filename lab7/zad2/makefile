CC=gcc
CFLAGS=-Wall
COOK=cook
DELIVERER=deliverer
MAIN=main
VALGRIND=

all: compile

compile:
	$(CC) $(CFLAGS) $(COOK).c -o $(COOK) -lpthread -lrt
	$(CC) $(CFLAGS) $(DELIVERER).c -o $(DELIVERER) -lpthread -lrt
	$(CC) $(CFLAGS) $(MAIN).c -o $(MAIN) -lpthread -lrt

clean:
	rm -f *.o *.a *.so $(COOK) $(DELIVERER) $(MAIN)