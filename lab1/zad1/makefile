CC=gcc
CFLAGS=-Wall
LIBRARY_NAME=lib

compile_static:
	$(CC) -c $(CFLAGS) $(LIBRARY_NAME).c
	ar cr lib$(LIBRARY_NAME).a $(LIBRARY_NAME).o

compile_shared:
	$(CC) -fPIC -c $(CFLAGS) $(LIBRARY_NAME).c
	$(CC) -shared -fPIC -o lib$(LIBRARY_NAME).so $(LIBRARY_NAME).o

clean:
	rm -f *.o *.a *.so