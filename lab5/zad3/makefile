CC=gcc
CFLAGS=-Wall
MAIN=main
PRODUCER=producer
CONSUMER=consumer
VALGRIND=

test: all
	./$(MAIN)

all: clean compile

compile:
	$(CC) $(CFLAGS) $(MAIN).c -o $(MAIN)
	$(CC) $(CFLAGS) $(PRODUCER).c -o $(PRODUCER)
	$(CC) $(CFLAGS) $(CONSUMER).c -o $(CONSUMER)

clean:
	rm -f *.o *.a *.so $(MAIN) $(PRODUCER) $(CONSUMER) fifo