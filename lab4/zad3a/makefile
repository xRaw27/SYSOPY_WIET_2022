CC=gcc
CFLAGS=-Wall
SENDER=sender
CATCHER=catcher
VALGRIND=

all: clean compile

compile:
	$(CC) $(CFLAGS) $(SENDER).c -o $(SENDER)
	$(CC) $(CFLAGS) $(CATCHER).c -o $(CATCHER)

clean:
	rm -f *.o *.a *.so $(SENDER) $(CATCHER)