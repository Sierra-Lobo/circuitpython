
LDFLAGS+=-lm -ggdb3
CFLAGS+= -Wextra -Wall -ggdb3


OBJECTS=igrf.o igrf-tst.o #igrfCoeffs.o

COEFF=igrf11coeffs.txt

all: igrf-tst# tags

#tags: $(wildcard *.c) $(wildcard *.h)
#	ctags $(wildcard *.c) $(wildcard *.h)

igrf-tst: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -g -o $@ -lm

#igrfCoeffs.c: extract-cords.py $(COEFF)
#	python3 extract-cords.py -o igrfCoeffs.c $(COEFF)

.PHONY:
test:	igrf-tst
	#@echo -e 'Old:\nx = 11443.719582 \ny = 4207.010590\nz = 55433.999964\nNew:';./igrf-tst
	@echo -e 'Old:\nx = 11443.716524\ny = 4207.008353\nz = 55434.001180\nNew:';./igrf-tst


.PHONY:
clean:
	rm -vf igrf-tst
	rm -vf $(OBJECTS)
	rm -vf igrfCoeffs.c



