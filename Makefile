# check usage of http://www.galassi.org/mark/mydocs/autoconf_tutorial_2.html#SEC2
all: clean razz razz_prof razz_fast
# assserts make code slower, remove them with -DNDEBUG

PROFILE=-pg -g2
FAST=-DNDEBUG -O3
CFLAGS=-lm -Wall -std=c99 --pedantic # not using ansi C
CC=gcc
RAZZ_FILES=razz.c razz_impl.c
FILES=razz.c razz_impl.c razz.h razz.py precision.c precision.py Makefile
FNAME=andrea_crotti.tar.gz
PHONIES = clean nose dist doc

razz:
	$(CC) $(CFLAGS) -o razz $(RAZZ_FILES)

razz_fast:
	$(CC) $(FAST) $(CFLAGS) -o razz_fast $(RAZZ_FILES)

razz_prof:
	$(CC) $(FAST) $(PROFILE) $(CFLAGS) -o razz_prof $(RAZZ_FILES)

precision:
	$(CC) $(CFLAGS) $(FAST) -o precision razz_impl.c precision.c

# PHONIES
doc:
	doxygen

dist:
	tar -cvzf $(FNAME) $(FILES)

nose:
	nosetests-2.6

clean:
	rm -f *.o razz razz_prof razz_fast precision

