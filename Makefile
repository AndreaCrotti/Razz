# check usage of http://www.galassi.org/mark/mydocs/autoconf_tutorial_2.html#SEC2
all: clean razz razz_prof razz_fast
# assserts make code slower, remove them with -DNDEBUG

PROFILE=-pg -g
FAST=-DNDEBUG -O3
CFLAGS=-lm -Wall -std=c99 --pedantic # not using ansi C
FILES=razz.c razz.h razz.py Makefile
FNAME=andrea_crotti.tar.gz

razz:
	gcc $(CFLAGS) -o razz razz.c

razz_fast:
	gcc $(FAST) $(CFLAGS) -o razz_fast razz.c

razz_prof:
	gcc $(PROFILE) $(CFLAGS) -o razz_prof razz.c

doc:
	doxygen

dist:
	tar -cvzf $(FNAME) $(FILES)

nose:
	nosetests-2.6

clean:
	rm -f *.o razz razz_prof razz_fast

