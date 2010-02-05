all: clean razz razzProf

PROFILE=-pg -g

razz:
	gcc -Wall -o razz razz.c

razzProf:
	gcc $(PROFILE) -Wall -o razz_prof razz.c

doc:
	doxygen

clean:
	rm -vf *.o razz razz_prof

