all: clean razz

razz:
	cc -Wall -o razz razz.c

clean:
	rm -vf *.o