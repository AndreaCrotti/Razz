all: razz razz_prof razz_fast
# assserts make code slower, remove them with -DNDEBUG

PROFILE=-pg -g2
FAST=-DNDEBUG -O3
CFLAGS=-lm -Wall -Wextra -std=c99 --pedantic
CC=gcc
RAZZ_FILES=razz.c razz.h
FNAME=andrea_crotti.tar

# set the razz objects here
razz razz_prof razz_fast: $(RAZZ_FILES)

razz_prof razz_fast: CFLAGS += $(FAST)
razz_prof: CFLAGS += $(PROFILE)

razz razz_prof razz_fast:
	$(CC) $(CFLAGS) -o $@ razz.c

doc: Doxyfile
	doxygen

# use git --archive maybe if possible?
dist: $(FNAME)

$(FNAME):
	git archive --format=tar -o $@ master

test:
	nosetests-2.6

clean:
	rm -f *.o razz razz_prof razz_fast $(FNAME)


.PHONY: doc clean test all dist
