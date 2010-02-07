all: clean razz razz_prof razz_fast
# assserts make code slower, remove them with -DNDEBUG

PROFILE=-pg -g
FAST=-DNDEBUG -O3
OPT=-lm -Wall
FILES=razz.c razz.py Makefile
FNAME=andrea_crotti.tar.gz

razz:
	gcc $(OPT) -o razz razz.c

razz_fast:
	gcc $(FAST) $(OPT) -o razz_fast razz.c

razz_prof:
	gcc $(PROFILE) $(OPT) -o razz_prof razz.c

doc:
	doxygen

dist:
	tar -cvzf $(FNAME) $(FILES)

clean:
	rm -vf *.o razz razz_prof razz_fast

