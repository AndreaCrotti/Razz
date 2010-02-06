all: clean doc razz razz_prof razz_fast
# assserts make code slower, remove them with -DNDEBUG

PROFILE=-pg -g
FAST=-DNDEBUG -O3

razz:
	gcc -Wall -o razz razz.c

razz_fast:
	gcc $(FAST) -Wall -o razz_fast razz.c

razz_prof:
	gcc $(PROFILE) -Wall -o razz_prof razz.c

doc:
	doxygen

clean:
	rm -vf *.o razz razz_prof

