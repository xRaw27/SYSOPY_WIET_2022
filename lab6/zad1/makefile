CC=gcc
CFLAGS=-Wall
SERVER=server
CLIENT=client
VALGRIND=

all: compile
	$(VALGRIND) ./$(SERVER)

compile:
	$(CC) $(CFLAGS) $(SERVER).c -o $(SERVER)
	$(CC) $(CFLAGS) $(CLIENT).c -o $(CLIENT)

clean:
	rm -f *.o *.a *.so $(SERVER) $(CLIENT)