all: rcex example

example: example.o
	gcc example.o -o example -Wall -O2

example.o: example.c
	gcc -c -O2 -Wall example.c

rcex: rcex.o 
	gcc rcex.o -o rcex.so -O2 -Wall -fPIC -shared -ldl -D_GNU_SOURCE

rcex.o: rcex.c
	gcc -c -O2 -Wall  rcex.c

clean:
	rm -f *.o *.so example