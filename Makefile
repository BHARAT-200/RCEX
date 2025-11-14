# --- Build RCEX + Example program ---

CC      = gcc
CFLAGS  = -Wall -O2 -I.
LDFLAGS = -shared -fPIC

all: librcex.so example

rcex.o: rcex.c rcex.h
	$(CC) -c rcex.c $(CFLAGS)

librcex.so: rcex.o
	$(CC) rcex.o -o librcex.so $(LDFLAGS)

clean:
	rm -f *.o *.so example encrypt

.PHONY: all clean

encrypt: encrypt.o rcex.o
	$(CC) encrypt.o rcex.o -o encrypt $(CFLAGS)