CC=gcc
CFLAGS=-Wall
SERVER=server
CLIENT=client
VALGRIND=

all: compile

compile:
	$(CC) $(CFLAGS) $(SERVER).c -o $(SERVER) -lpthread
	$(CC) $(CFLAGS) $(CLIENT).c -o $(CLIENT) -lpthread

clean:
	rm -f *.o *.a *.so $(SERVER) $(CLIENT)
