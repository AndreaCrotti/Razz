# check usage of http://www.galassi.org/mark/mydocs/autoconf_tutorial_2.html#SEC2
all: clean razz razz_prof razz_fast
# assserts make code slower, remove them with -DNDEBUG
#TODO: switch to automake 

PROFILE=-pg -g2
FAST=-DNDEBUG -O3
CFLAGS=-lm -Wall -Wextra -std=c99 --pedantic
CC=gcc
RAZZ_FILES=razz.c
FILES=razz.c razz.h razz.py precision.py Makefile
FNAME=andrea_crotti.tar.gz
PHONIES = clean nose dist doc

# set the razz objects here
razz: $(RAZZ_FILES)
	$(CC) $(CFLAGS) -o razz $(RAZZ_FILES)

razz_fast: $(RAZZ_FILES)
	$(CC) $(FAST) $(CFLAGS) -o razz_fast $(RAZZ_FILES)

razz_prof: $(RAZZ_FILES)
	$(CC) $(FAST) $(PROFILE) $(CFLAGS) -o razz_prof $(RAZZ_FILES)

# TODO: add to the PHONIES
doc:
	doxygen

dist:
	tar -cvzf $(FNAME) $(FILES)

test:
	nosetests-2.6

clean:
	rm -f *.o razz razz_prof razz_fast

