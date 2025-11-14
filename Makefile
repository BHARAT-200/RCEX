CC      = gcc
CFLAGS  = -Wall -O2 -I.
LDFLAGS = -shared -fPIC

all: encrypt

# Object Files
rcex.o: rcex.c rcex.h
	$(CC) -c rcex.c $(CFLAGS)

encrypt.o: encrypt.c rcex.h
	$(CC) -c encrypt.c $(CFLAGS)

# CLI file encrypt ool
encrypt: encrypt.o rcex.o
	$(CC) encrypt.o rcex.o -o encrypt $(CFLAGS)

# Build Shared Library
librcex.so: rcex.o
	$(CC) rcex.o -o librcex.so $(LDFLAGS)

clean:
	rm -f *.o *.so encrypt example

.PHONY: all clean